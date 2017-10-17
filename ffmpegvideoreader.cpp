#include "ffmpegvideoreader.h"

#include <iostream>

#define CRITICAL_ERROR(x) \
{ \
    std::lock_guard<std::mutex> locker(m_ioMutex); \
    m_errorString = x; \
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
    m_videoStreamIdx(-1)
{
    av_register_all();
    avformat_network_init();
    avdevice_register_all();
    av_log_set_level(AV_LOG_QUIET);
}

FfmpegVideoReader::~FfmpegVideoReader()
{
    close();
}

bool FfmpegVideoReader::open(const std::string &path)
{
    m_stop = false;
    m_path = path;
    m_videoStreamIdx = -1;

    AVDictionary *opts = NULL;

    av_dict_set(&opts, "flags", "low_delay", 0);
    av_dict_set(&opts, "fflags", "nobuffer", 0);

    AVFormatContext *formatContext = avformat_alloc_context();

    /* callback */
    formatContext->interrupt_callback.callback = &interruptCallback;
    formatContext->interrupt_callback.opaque = &m_stop;

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

    AVStream *stream = m_formatContext->streams[m_videoStreamIdx];
    if(!stream)
        CRITICAL_ERROR("Could not find audio or video stream in the input, aborting");

    /* find decoder for the stream */
    m_codecContext = SmartCodecContext(stream->codec, codecContextDeleter);
    AVCodec *codec = avcodec_find_decoder(m_codecContext->codec_id);
    if(!codec)
        CRITICAL_ERROR("Failed to find codec");

    /* Init the decoders, with or without reference counting */
    av_dict_set(&opts, "refcounted_frames", "0", 0);
    if(avcodec_open2(m_codecContext.get(), codec, &opts) < 0)
        CRITICAL_ERROR("Failed to open codec");

    m_isOpen = true;
    av_dict_free(&opts);

    m_readingThread = std::thread([this] {
        this->readingThreadImpl();
    });
    m_decodingThread = std::thread([this] {
        this->decodingThreadImpl();
    });

    return true;
}

SmartFrame FfmpegVideoReader::getFrame()
{
    std::lock_guard<std::mutex> locker(m_ioMutex);
    SmartFrame frame =  m_frame;
    m_frame = 0;
    return frame;
}

bool FfmpegVideoReader::isRealtime() const
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
    m_stop = true;
    m_queueNotify.notify_all();
    if(m_readingThread.joinable())
        m_readingThread.join();
    if(m_decodingThread.joinable())
        m_decodingThread.join();
}

bool FfmpegVideoReader::isOpen() const
{
    return m_isOpen;
}

double FfmpegVideoReader::getFps() const
{
    AVStream *videoStream = m_formatContext->streams[m_videoStreamIdx];
    const double eps_zero = 0.000025;
    double fps = av_q2d(videoStream->r_frame_rate);

    if(fps < eps_zero)
    {
        fps = av_q2d(videoStream->avg_frame_rate);
        std::cerr << "r_frame_rate is wrong, use avg_frame_rate, fps: " << fps << std::endl;
    }

    if(fps < eps_zero)
    {
        fps = 1.0 / av_q2d(m_codecContext->time_base);
        std::cerr << "avg_frame_rate is wrong, use codec time_base, fps: " << fps << std::endl;
    }

    return fps;
}

std::string FfmpegVideoReader::getErrorString() const
{
    std::lock_guard<std::mutex> locker(m_ioMutex);
    return m_errorString;
}

void FfmpegVideoReader::readingThreadImpl()
{
    int interval = 1000.0 / getFps();
    bool realtime = isRealtime();
    while(!m_stop)
    {
        auto tp1 = hclock::now();
        SmartPacket packet(new AVPacket(), packetDeleter);
        if(av_read_frame(m_formatContext.get(), packet.get()) >= 0)
        {
            auto tp2 = hclock::now();
            if(packet->stream_index == m_videoStreamIdx)
            {
                m_queueMutex.lock();
                m_queue.push(packet);
                m_queueMutex.unlock();
                m_queueNotify.notify_all();
                if(!realtime)
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval - duration(tp1, tp2)));
            }
        }
        else
            m_isOpen = false;
    }
}

void FfmpegVideoReader::decodingThreadImpl()
{
    std::unique_lock<std::mutex> lock(m_queueMutex);
    while(!m_stop)
    {
        m_queueNotify.wait(lock);
        while(m_queue.size() > 0)
        {
            SmartPacket packet = m_queue.front();
            SmartFrame frame(av_frame_alloc(), frameDeleter);
            m_queue.pop();
            int got_frame = 0;
            if(avcodec_decode_video2(m_codecContext.get(), frame.get(), &got_frame, packet.get()) < 0)
            {
                std::cout << "Warning: Error when decoding video frame" << std::endl;
            }
            else if(got_frame == 1)
            {
                std::lock_guard<std::mutex> locker(m_ioMutex);
                m_frame = frame;
            }
        }
    }
}

int FfmpegVideoReader::duration(const hclock::time_point &tp1, const hclock::time_point &tp2)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp2 - tp1).count();
}
