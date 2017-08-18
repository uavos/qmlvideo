#include "smartffmpeg.h"

#include <iostream>

void frameDeleter(AVFrame *frame)
{
    av_frame_free(&frame);
}

void swsContextDeleter(SwsContext *context)
{
    sws_freeContext(context);
}

void formatContextDeleter(AVFormatContext *context)
{
    AVStream *stream = (AVStream*)context->opaque;
    if(stream && stream->codec)
        avcodec_close(stream->codec);
    avformat_close_input(&context);
}

void packetDeleter(AVPacket *packet)
{
    av_free_packet(packet);
    delete packet;
}
