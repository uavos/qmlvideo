#ifndef FFMPEGPLAYER_H
#define FFMPEGPLAYER_H

#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QMediaPlayer>
#include <QTimer>
#include "videothread.h"
#include "osdmenu.h"

class FfmpegPlayer : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(OsdMenu* osdMenu READ osdMenu WRITE setOsdMenu)
    Q_PROPERTY(QString source READ getSource WRITE setSource)
    Q_PROPERTY(int playbackState READ getPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QString stateText READ getStateText NOTIFY stateTextChanged)
    Q_PROPERTY(int error READ getError NOTIFY errorOccured)
    Q_PROPERTY(QString errorString READ getErrorString NOTIFY errorStringChanged)
    Q_PROPERTY(int reconnectTimeout READ getReconnectTimeout WRITE setReconnectTimeout)
    Q_PROPERTY(QString recdir READ getRecdir WRITE setRecdir)
    Q_PROPERTY(QString photodir READ getPhotoDir WRITE setPhotoDir)

    static void registerQmlType();

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    Q_INVOKABLE void snapshot();

    Q_INVOKABLE void enableRecord(bool b);

    explicit FfmpegPlayer(QObject *parent = 0);
    ~FfmpegPlayer();

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface* s);

    QString getSource() const;
    void setSource(const QString &value);

    int getPlaybackState() const;
    QString getStateText() const;

    int getError() const;

    QString getErrorString() const;

    int getReconnectTimeout() const;
    void setReconnectTimeout(int reconnectTimeout);

    QString getRecdir();
    void setRecdir(const QString &recdir);

    QString getPhotoDir();
    void setPhotoDir(const QString &photodir);

    bool stabilizierEnabled();
    void setStabilizierEnabled(bool b);

    OsdMenu *osdMenu() const;
    void setOsdMenu(OsdMenu *osdMenu);

private slots:
    void onFrameReceived(QImage frame);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void onErrorOccurred(QString errorString);
    void setPlaybackState(const QMediaPlayer::State &playbackState);
    void setStateText(const QString &text);
    void setError(const QMediaPlayer::Error &error);
    void setErrorString(const QString &errorString);
    void onReconnectTimerTimeout();

private:
    OsdMenu *m_osdMenu;
    QAbstractVideoSurface* m_surface;
    QString m_source;
    VideoThread m_videoThread;
    int m_playbackState;
    int m_error;
    int m_reconnectTimeout;
    QString m_errorString;
    QString m_stateText;
    QString m_photoDir;
    QImage m_frame;
    QTimer m_reconnectTimer;
    QString m_flags;

signals:
    void playbackStateChanged();
    void errorOccured();
    void errorStringChanged();
    void enableStabilizierChanged();
    void stateTextChanged();
};

#endif // FFMPEGPLAYER_H
