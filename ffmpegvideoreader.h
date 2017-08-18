#ifndef FFMPEGVIDEOREADER_H
#define FFMPEGVIDEOREADER_H

#include <string>
#include <atomic>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
}

#include "smartffmpeg.h"

class FfmpegVideoReader
{
public:
    FfmpegVideoReader();

    void setInterruptFlag(std::atomic_bool *stop);

    bool open(const std::string &path);
    bool read(SmartFrame frame);
    bool isRealtime();
    void close();

    bool isOpen() const;
    double getFps();

    void setNoBuffer(bool noBuffer);
    void setLowDelay(bool lowDelay);

private:
    bool m_isOpen;
    std::string m_path;
    std::atomic_bool *m_stop;
    std::atomic_bool m_noBuffer;
    std::atomic_bool m_lowDelay;
    SmartFormatContext m_formatContext;
    int m_videoStreamIdx;
    double r2d(AVRational r);
};

#endif // FFMPEGVIDEOREADER_H
