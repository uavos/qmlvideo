#include "ffmpegvideoreader.h"

#include <iostream>

#define CRITICAL_ERROR(x) \
{ \
    std::cout << x << std::endl; \
    av_dict_free(&opts); \
    return false; \
    }

static int interruptCallback(void *opaque)
{
    std::atomic_bool *stop = static_cast<std::atomic_bool*>(opaque);
    return *stop;
}

FfmpegVideoReader::FfmpegVideoReader():
    m_isOpen(false),
    m_stop(0),
    m_noBuffer(false),
    m_lowDelay(false),
    m_videoStreamIdx(-1)
{
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
}

void FfmpegVideoReader::setInterruptFlag(std::atomic_bool *stop)
{
    m_stop = stop;
}

bool FfmpegVideoReader::open(const std::string &path)
{
    m_path = path;

    m_videoStreamIdx = -1;

    AVDictionary *opts = NULL;

    if(m_lowDelay)
    {
        av_dict_set(&opts, "flags", "low_delay", 0);
        std::cout << "Low delay flag detected" << std::endl;
    }
    if(m_noBuffer)
    {
        av_dict_set(&opts, "fflags", "nobuffer", 0);
        std::cout << "No buffer flag detected" << std::endl;
    }

    AVFormatContext *formatContext = avformat_alloc_context();

    /* callback */
    formatContext->interrupt_callback.callback = &interruptCallback;
    formatContext->interrupt_callback.opaque = m_stop;

    /* open input file, and allocate format context */
    if(avformat_open_input(&formatContext, m_path.c_str(), NULL, &opts) < 0)
        CRITICAL_ERROR("Could not open source file");

    m_formatContext = SmartFormatContext(formatContext, formatContextDeleter);

    /* retrieve stream information */
    if(avformat_find_stream_info(m_formatContext.get(), NULL) < 0)
        CRITICAL_ERROR("Could not find stream information");

    /* find best stream */
    m_videoStreamIdx = av_find_best_stream(m_formatContext.get(), AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if(m_videoStreamIdx < 0)
        CRITICAL_ERROR("Could not find video stream in input file");

    AVStream *videoStream = m_formatContext->streams[m_videoStreamIdx];
    if(!videoStream)
        CRITICAL_ERROR("Could not find audio or video stream in the input, aborting");
    m_formatContext->opaque = videoStream;

    /* find decoder for the stream */
    AVCodecContext *codecContext = videoStream->codec;
    AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
    if(!codec)
        CRITICAL_ERROR("Failed to find codec");

    /* Init the decoders, with or without reference counting */
    av_dict_set(&opts, "refcounted_frames", "0", 0);
    if(avcodec_open2(codecContext, codec, &opts) < 0)
        CRITICAL_ERROR("Failed to open codec");

    m_isOpen = true;
    av_dict_free(&opts);
    return true;
}

bool FfmpegVideoReader::read(SmartFrame frame)
{
    SmartPacket packet(new AVPacket(), packetDeleter);
    /* read frames from the file */
    if(av_read_frame(m_formatContext.get(), packet.get()) < 0)
        return false;
    int got_frame = 0;
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIdx];
    AVCodecContext *codecContext = videoStream->codec;
    if(packet->stream_index == m_videoStreamIdx)
    {
        /* decode video frame */
        int ret = avcodec_decode_video2(codecContext, frame.get(), &got_frame, packet.get());
        if(ret < 0)
        {
            std::cout << "Warning: Error when decoding video frame" << std::endl;
            return false;
        }
    }
    return got_frame > 0;
}

bool FfmpegVideoReader::isRealtime()
{
    std::string formatName = std::string(m_formatContext->iformat->name);
    return formatName == "rtsp" ||
            formatName == "rtp" ||
            formatName == "sdp" ||
            formatName == "video4linux2,v4l2" ||
            !formatName.compare(0, 4, "rtp:") ||
            !formatName.compare(0, 4, "udp:");
}

void FfmpegVideoReader::close()
{
    m_isOpen = false;
}

bool FfmpegVideoReader::isOpen() const
{
    return m_isOpen;
}

double FfmpegVideoReader::getFps()
{
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIdx];
    const double eps_zero = 0.000025;
    double fps = r2d(videoStream->r_frame_rate);

    if(fps < eps_zero)
    {
        fps = r2d(videoStream->avg_frame_rate);
        std::cerr << "r_frame_rate is wrong, use avg_frame_rate, fps: " << fps << std::endl;
    }

    if(fps < eps_zero)
    {
        fps = 1.0 / r2d(videoStream->codec->time_base);
        std::cerr << "avg_frame_rate is wrong, use codec time_base, fps: " << fps << std::endl;
    }

    return fps;
}

void FfmpegVideoReader::setNoBuffer(bool noBuffer)
{
    m_noBuffer = noBuffer;
}

void FfmpegVideoReader::setLowDelay(bool lowDelay)
{
    m_lowDelay = lowDelay;
}

double FfmpegVideoReader::r2d(AVRational r)
{
    if(r.num == 0 || r.den == 0)
        return 0;
    else
        return (double)r.num / (double)r.den;
}
