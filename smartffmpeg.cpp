#include "smartffmpeg.h"

#include <iostream>

void codecContextDeleter(AVCodecContext *codecContext)
{
}

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
    avformat_close_input(&context);
}

void packetDeleter(AVPacket *packet)
{
    av_packet_unref(packet);
    delete packet;
}
