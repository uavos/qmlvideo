#ifndef SMARTFFMPEG_H
#define SMARTFFMPEG_H

#include <memory>
#include <functional>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
}

void frameDeleter(AVFrame *frame);
void swsContextDeleter(SwsContext *context);
void formatContextDeleter(AVFormatContext *context);
void packetDeleter(AVPacket *packet);

using SmartFrame = std::shared_ptr<AVFrame>;
using SmartSwsContext = std::shared_ptr<SwsContext>;
using SmartFormatContext = std::shared_ptr<AVFormatContext>;
using SmartPacket = std::shared_ptr<AVPacket>;

#endif // SMARTFFMPEG_H

