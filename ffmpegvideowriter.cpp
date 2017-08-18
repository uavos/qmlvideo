#include "ffmpegvideowriter.h"

#include <iostream>

#define CRITICAL_ERROR(x) \
{ \
    std::cerr << x << std::endl; \
    av_dict_free(&opts); \
    return false; \
    }

#define CRITICAL_ERROR_WRITE(x) \
{ \
    std::cerr << x << std::endl; \
    return false; \
    }

FfmpegVideoWriter::FfmpegVideoWriter():
    m_isOpen(false),
    m_codecId(AV_CODEC_ID_H264),
    m_width(-1),
    m_height(-1),
    m_framerate(-1),
    m_pts(0)
{
}

bool FfmpegVideoWriter::open(const std::string &path, AVCodecID codecId, int width, int height, int framerate)
{
    m_path = path;
    m_codecId = codecId;
    m_width = width;
    m_height = height;
    m_framerate = framerate;

    int ret;
    AVDictionary *opts = NULL;
    if(codecId == AV_CODEC_ID_H264)
    {
        av_dict_set(&opts, "preset", "ultrafast", 0);
        av_dict_set(&opts, "tune", "zerolatency", 0);
    }

    AVFormatContext *formatContext;
    avformat_alloc_output_context2(&formatContext, NULL, NULL, m_path.c_str());
    m_formatContext = SmartFormatContext(formatContext, formatContextDeleter);
    if(!m_formatContext)
        CRITICAL_ERROR("Could not allocate format context");

    /* find the encoder */
    AVCodec *codec = avcodec_find_encoder_by_name("libx264rgb");
    if(!codec)
        CRITICAL_ERROR("Could not find encoder");

    AVStream *stream = avformat_new_stream(m_formatContext.get(), codec);
    if(!stream)
        CRITICAL_ERROR("Could not allocate stream");
    m_formatContext->opaque = stream;
    stream->id = m_formatContext->nb_streams - 1;

    AVCodecContext *codecContext = stream->codec;
    if(!codecContext)
        CRITICAL_ERROR("Could not allocate codec context");

    if(codec->type == AVMEDIA_TYPE_VIDEO)
    {
        codecContext->codec_id = m_codecId;
        /* Resolution must be a multiple of two. */
        codecContext->width = m_width;
        codecContext->height = m_height;
        /* timebase: This is the fundamental unit of time (in seconds) in terms
             * of which frame timestamps are represented. For fixed-fps content,
             * timebase should be 1/framerate and timestamp increments should be
             * identical to 1. */
        stream->time_base = { 1, m_framerate };
        codecContext->time_base = stream->time_base;
        codecContext->gop_size = 12; /* emit one intra frame every twelve frames at most */
        codecContext->pix_fmt = AV_PIX_FMT_RGB24;
    }
    else
        CRITICAL_ERROR("Unknown codec type");

    /* Some formats want stream headers to be separate. */
    if(m_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    /* open the codec */
    ret = avcodec_open2(codecContext, codec, &opts);
    if(ret < 0)
        CRITICAL_ERROR("Could not open video codec");

    ret = avio_open(&m_formatContext->pb, m_path.c_str(), AVIO_FLAG_WRITE);
    if(ret < 0)
        CRITICAL_ERROR("Could not open file");

    ret = avformat_write_header(m_formatContext.get(), &opts);
    if(ret < 0)
        CRITICAL_ERROR("Could not write header");

    m_isOpen = true;
    av_dict_free(&opts);
    return true;
}

bool FfmpegVideoWriter::write(SmartFrame frame)
{
    AVStream *stream = (AVStream*)m_formatContext->opaque;
    AVCodecContext *codecContext = stream->codec;

    if(av_compare_ts(m_pts, codecContext->time_base,
                     INT64_MAX, (AVRational{1, 1})) >= 0)
        return false;
    SmartSwsContext swsContext(sws_getContext(frame->width, frame->height,
                                              AVPixelFormat(frame->format),
                                              m_width, m_height, AV_PIX_FMT_RGB24,
                                              SWS_FAST_BILINEAR, 0, 0, 0),
                               swsContextDeleter);

    SmartFrame temp(av_frame_alloc(), frameDeleter);
    if(!temp)
        CRITICAL_ERROR_WRITE("Cannot alloc frame");
    temp->format = AV_PIX_FMT_RGB24;
    temp->width = m_width;
    temp->height = m_height;
    temp->pts = m_pts++;
    int ret = av_frame_get_buffer(temp.get(), 32);
    if(ret < 0)
        CRITICAL_ERROR_WRITE("Cannot allocate frame data");
    sws_scale(swsContext.get(), frame->data, frame->linesize, 0,
              frame->height, temp->data, temp->linesize);

    int got_packet = 0;
    SmartPacket packet(new AVPacket(), packetDeleter);
    packet->data = 0;
    packet->size = 0;
    /* encode the image */
    ret = avcodec_encode_video2(codecContext, packet.get(), temp.get(), &got_packet);
    if(ret < 0)
        CRITICAL_ERROR_WRITE("Error encoding video frame");
    if(got_packet)
    {
        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(packet.get(), codecContext->time_base, stream->time_base);
        packet->stream_index = stream->index;
        /* Write the compressed frame to the media file. */
        ret = av_interleaved_write_frame(m_formatContext.get(), packet.get());
    }
    if(ret < 0)
        CRITICAL_ERROR_WRITE("Error while writing video frame");

    return true;
}

void FfmpegVideoWriter::close()
{
    av_write_trailer(m_formatContext.get());
    m_width = -1;
    m_height = -1;
    m_framerate = -1;
    m_pts = 0;
    m_isOpen = false;
}

bool FfmpegVideoWriter::isOpen() const
{
    return m_isOpen;
}
