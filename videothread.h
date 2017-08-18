#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QAbstractSocket>
#include "ffmpegvideowriter.h"
#include "ffmpegvideoreader.h"
#include "osdmenu.h"
#include <atomic>
#include "smartffmpeg.h"

class VideoThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoThread(QObject *parent = 0);
    QString inputUrl();
    void setInputUrl(const QString &inputUrl);
    QImage getFrame();
    void stop();
    void enableWriter();
    void disableWriter();
    QString getRecdir();
    void setRecdir(const QString &recdir);

    void setOsdMenu(OsdMenu *osdMenu);

    void setNoBuffer(bool noBuffer);
    void setLowDelay(bool lowDelay);

protected:
    void run();

private:
    std::atomic_bool m_stop;
    std::atomic_bool m_enableWriter;
    std::atomic_bool m_noBuffer;
    std::atomic_bool m_lowDelay;
    QString m_inputUrl;
    QString m_recdir;
    QMutex m_ioMutex;
    QImage m_frame;
    OsdMenu *m_osdMenu;
    void qimage2avframe(SmartFrame frame, const QImage &image);
    void checkWriterState(FfmpegVideoWriter &writer, int width, int height, double fps);

signals:
    void frameReceived(QImage image);
    void errorOccured(QString str);
    void stateChanged(QAbstractSocket::SocketState state);
};

#endif // VIDEOTHREAD_H
