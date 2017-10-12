#include "videothread.h"

#include <QDir>
#include <QDateTime>
#include <chrono>
#include <iostream>

VideoThread::VideoThread(QObject *parent):
    QThread(parent),
    m_stop(false),
    m_enableWriter(false),
    m_inputUrl("udp://127.0.0.1:30010"),
    m_recdir(QDir::homePath()),
    m_osdMenu(0)
{
    av_log_set_level(AV_LOG_ERROR);
}

void VideoThread::run()
{
    m_stop = false;

    FfmpegVideoReader reader;
    FfmpegVideoWriter writer;

    emit stateChanged(QAbstractSocket::ConnectingState);

    m_ioMutex.lock();
    reader.open(m_inputUrl.toStdString());
    m_ioMutex.unlock();

    if(!reader.isOpen())
    {
        reader.close();
        return;
    }

    emit stateChanged(QAbstractSocket::ConnectedState);

    while(!m_stop)
    {
        //reader
        SmartFrame frame = reader.getFrame();
        if(frame)
        {
            SmartFrame frameRgb32(av_frame_alloc(), frameDeleter);
            frameRgb32->width = frame->width;
            frameRgb32->height = frame->height;
            frameRgb32->format = AV_PIX_FMT_RGB32;
            av_frame_get_buffer(frameRgb32.get(), 32);

            checkWriterState(writer, frame->width, frame->height, reader.getFps());

            SmartSwsContext swsContext(sws_getContext(frame->width, frame->height,
                                                      AVPixelFormat(frame->format),
                                                      frameRgb32->width, frameRgb32->height,
                                                      AVPixelFormat(frameRgb32->format),
                                                      SWS_FAST_BILINEAR, 0, 0, 0),
                                       swsContextDeleter);

            sws_scale(swsContext.get(), frame->data, frame->linesize, 0, frame->height,
                      frameRgb32->data, frameRgb32->linesize);

            QImage rgb32Wrapper(frameRgb32->data[0], frameRgb32->width, frameRgb32->height,
                    QImage::Format_RGB32);

            QImage rgb32Copy = rgb32Wrapper.copy();
            if(writer.isOpen())
            {
                QImage osdCopy = rgb32Copy.copy();
                if(m_osdMenu)
                    m_osdMenu->drawOsdMenu(&osdCopy);
                SmartFrame temp(av_frame_alloc(), frameDeleter);
                qimage2avframe(temp, osdCopy);
                writer.write(temp);
            }

            emit frameReceived(rgb32Copy);
        }
        msleep(1);
    }

    if(reader.isOpen())
        reader.close();
    if(writer.isOpen())
        writer.close();

    emit stateChanged(QAbstractSocket::UnconnectedState);
}

void VideoThread::setOsdMenu(OsdMenu *osdMenu)
{
    m_osdMenu = osdMenu;
}

QString VideoThread::getRecdir()
{
    QMutexLocker locker(&m_ioMutex);
    return m_recdir;
}

void VideoThread::setRecdir(const QString &recdir)
{
    QDir dir;
    if(dir.mkpath(recdir))
    {
        dir.cd(recdir);
        QMutexLocker locker(&m_ioMutex);
        m_recdir = dir.absolutePath();
    }
    else
    {
        QString message = "Can't create path %1, using home dir...";
        std::cerr << message.arg(recdir).toStdString() << std::endl;
        QMutexLocker locker(&m_ioMutex);
        m_recdir = QDir::homePath();
    }
}

QString VideoThread::inputUrl()
{
    QMutexLocker locker(&m_ioMutex);
    return m_inputUrl;
}

void VideoThread::setInputUrl(const QString &inputUrl)
{
    QMutexLocker locker(&m_ioMutex);
    m_inputUrl = inputUrl;
}

QImage VideoThread::getFrame()
{
    QMutexLocker locker(&m_ioMutex);
    return m_frame;
}

void VideoThread::enableWriter()
{
    m_enableWriter = true;
}

void VideoThread::disableWriter()
{
    m_enableWriter = false;
}

void VideoThread::stop()
{
    m_stop = true;
}

void VideoThread::qimage2avframe(SmartFrame frame, const QImage &image)
{
    frame->format = AV_PIX_FMT_RGB32;
    frame->width = image.width();
    frame->height = image.height();
    av_frame_get_buffer(frame.get(), 32);
    memcpy(frame->data[0], image.bits(), image.byteCount());
}

void VideoThread::checkWriterState(FfmpegVideoWriter &writer, int width, int height, double fps)
{
    if(m_enableWriter && !writer.isOpen())
    {
        QString currentDateTime = QDateTime::currentDateTime().toString("dd_mm_yyyyThh_mm_ss_zzz");
        QString filename = m_recdir + "/" + currentDateTime + ".avi";
        writer.open(filename.toStdString(), AV_CODEC_ID_H264, width, height, fps);
    }

    if(!m_enableWriter && writer.isOpen())
    {
        writer.close();
    }
}
