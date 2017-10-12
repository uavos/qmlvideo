#ifndef FFMPEGVIDEOREADER_H
#define FFMPEGVIDEOREADER_H

#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

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
    ~FfmpegVideoReader();

    bool open(const std::string &path);
    SmartFrame getFrame();
    void close();

    bool isRealtime() const;
    bool isOpen() const;
    double getFps() const;
    std::string getErrorString() const;

private:
    std::atomic_bool m_stop;
    std::atomic_bool m_isOpen;
    std::string m_path;
    std::string m_errorString;
    SmartFormatContext m_formatContext;
    SmartCodecContext m_codecContext;
    int m_videoStreamIdx;
    SmartFrame m_frame;
    mutable std::mutex m_ioMutex;

    std::queue<SmartPacket> m_queue;
    std::mutex m_queueMutex;
    std::condition_variable m_queueNotify;
    std::thread m_readingThread;
    std::thread m_decodingThread;

    void readingThreadImpl();
    void decodingThreadImpl();

    using hclock = std::chrono::high_resolution_clock;
    int duration(const hclock::time_point &tp1, const hclock::time_point &tp2);
};

#endif // FFMPEGVIDEOREADER_H
