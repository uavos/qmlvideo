#ifndef FFMPEGVIDEOWRITER_H
#define FFMPEGVIDEOWRITER_H

#include <string>

extern "C"
{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavcodec/avcodec.h>
}

#include "smartffmpeg.h"

class FfmpegVideoWriter
{
public:
    FfmpegVideoWriter();

    bool open(const std::string &path, AVCodecID codecId, int width, int height, int framerate);
    bool write(SmartFrame frame);
    void close();

    bool isOpen() const;

private:
    bool m_isOpen;
    std::string m_path;
    AVCodecID m_codecId;
    SmartFormatContext m_formatContext;
    int m_width;
    int m_height;
    int m_framerate;
    int64_t m_pts;
};

#endif // FFMPEGVIDEOWRITER_H
