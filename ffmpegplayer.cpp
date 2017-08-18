#include "ffmpegplayer.h"

#include <QtQml/qqml.h>

#include <QDir>
#include <QDateTime>
#include <iostream>

void FfmpegPlayer::registerQmlType()
{
    qmlRegisterType<FfmpegPlayer>("FfmpegPlayer", 10, 0, "FfmpegPlayer");
}

FfmpegPlayer::FfmpegPlayer(QObject *parent):
    QObject(parent),
    m_osdMenu(0),
    m_surface(0),
    m_playbackState(QMediaPlayer::StoppedState),
    m_error(QMediaPlayer::NoError),
    m_reconnectTimeout(5000),
    m_stateText("Unconnected"),
    m_photoDir(QDir::homePath()),
    m_lowDelay(false),
    m_noBuffer(false)
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    connect(&m_videoThread, SIGNAL(errorOccured(QString)), this, SLOT(onErrorOccurred(QString)));
    connect(&m_videoThread, SIGNAL(frameReceived(QImage)), this, SLOT(onFrameReceived(QImage)));
    connect(&m_videoThread, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    connect(&m_reconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectTimerTimeout()));
}

void FfmpegPlayer::play()
{
    m_videoThread.stop();
    if(!m_videoThread.wait(300))
        m_videoThread.terminate();
    m_error = QMediaPlayer::NoError;
    m_errorString.clear();
    m_videoThread.setInputUrl(m_source);
    m_videoThread.start();
    m_reconnectTimer.start(m_reconnectTimeout);
}

void FfmpegPlayer::stop()
{
    m_videoThread.stop();
    if(!m_videoThread.wait(300))
        m_videoThread.terminate();
    m_surface->stop();
}

void FfmpegPlayer::snapshot()
{
    if(m_frame.isNull())
        return;

    if(m_osdMenu)
        m_osdMenu->drawOsdMenu(&m_frame);

    QString filename = QDateTime::currentDateTime().toString("dd_mm_yyyyThh_mm_ss_zzz");

    QString filepath = m_photoDir + "/" + filename + ".png";
    m_frame.save(filepath);
}

void FfmpegPlayer::enableRecord(bool b)
{
    if(b)
        m_videoThread.enableWriter();
    else
        m_videoThread.disableWriter();
}

FfmpegPlayer::~FfmpegPlayer()
{
    stop();
}

QAbstractVideoSurface* FfmpegPlayer::videoSurface() const
{
    return m_surface;
}

void FfmpegPlayer::setVideoSurface(QAbstractVideoSurface* s)
{
    m_surface = s;
}

void FfmpegPlayer::onFrameReceived(QImage frame)
{
    if(!m_surface)
        return;
    if(!m_surface->isActive())
    {
        m_surface->start(QVideoSurfaceFormat(frame.size(), QVideoFrame::Format_RGB32));
    }

    m_reconnectTimer.start(m_reconnectTimeout);
    m_frame = frame;
    m_surface->present(QVideoFrame(frame));
}

void FfmpegPlayer::onStateChanged(QAbstractSocket::SocketState socketState)
{
    if(socketState == QAbstractSocket::ConnectedState)
    {
        setPlaybackState(QMediaPlayer::PlayingState);
        setStateText("Connected");
    }
    else if(socketState == QAbstractSocket::UnconnectedState)
    {
        setPlaybackState(QMediaPlayer::StoppedState);
        setStateText("Unconnected");
    }
    else if(socketState == QAbstractSocket::ConnectingState)
    {
        setStateText("Connecting...");
    }
}

void FfmpegPlayer::onErrorOccurred(QString errorString)
{
    setErrorString(errorString);
    setError(QMediaPlayer::ServiceMissingError);
}

QString FfmpegPlayer::getErrorString() const
{
    return m_errorString;
}

void FfmpegPlayer::setErrorString(const QString &errorString)
{
    m_errorString = errorString;
    emit errorStringChanged();
}

void FfmpegPlayer::onReconnectTimerTimeout()
{
    play();
}

bool FfmpegPlayer::getNoBuffer() const
{
    return m_noBuffer;
}

void FfmpegPlayer::setNoBuffer(bool noBuffer)
{
    m_noBuffer = noBuffer;
    m_videoThread.setNoBuffer(noBuffer);
}

bool FfmpegPlayer::getLowDelay() const
{
    return m_lowDelay;
}

void FfmpegPlayer::setLowDelay(bool lowDelay)
{
    m_lowDelay = lowDelay;
    m_videoThread.setLowDelay(lowDelay);
}

OsdMenu *FfmpegPlayer::osdMenu() const
{
    return m_osdMenu;
}

void FfmpegPlayer::setOsdMenu(OsdMenu *osdMenu)
{
    m_osdMenu = osdMenu;
    m_videoThread.setOsdMenu(osdMenu);
}

QString FfmpegPlayer::getRecdir()
{
    return m_videoThread.getRecdir();
}

void FfmpegPlayer::setRecdir(const QString &recdir)
{
    m_videoThread.setRecdir(recdir);
}

QString FfmpegPlayer::getPhotoDir()
{
    return m_photoDir;
}

void FfmpegPlayer::setPhotoDir(const QString &photodir)
{
    QDir dir;
    if(dir.mkpath(photodir))
    {
        dir.cd(photodir);
        m_photoDir = dir.absolutePath();
    }
    else
    {
        QString message = "Can't create path %1, using home dir...";
        std::cerr << message.arg(photodir).toStdString() << std::endl;
        m_photoDir = QDir::homePath();
    }
}

int FfmpegPlayer::getReconnectTimeout() const
{
    return m_reconnectTimeout;
}

void FfmpegPlayer::setReconnectTimeout(int reconnectTimeout)
{
    m_reconnectTimeout = reconnectTimeout;
}

int FfmpegPlayer::getError() const
{
    return m_error;
}

void FfmpegPlayer::setError(const QMediaPlayer::Error &error)
{
    m_error = error;
    emit errorOccured();
}

int FfmpegPlayer::getPlaybackState() const
{
    return m_playbackState;
}

QString FfmpegPlayer::getStateText() const
{
    return m_stateText;
}

void FfmpegPlayer::setPlaybackState(const QMediaPlayer::State &playbackState)
{
    m_playbackState = playbackState;
    emit playbackStateChanged();
}

void FfmpegPlayer::setStateText(const QString &text)
{
    if(text != m_stateText)
    {
        m_stateText = text;
        emit stateTextChanged();
    }
}

QString FfmpegPlayer::getSource() const
{
    return m_source;
}

void FfmpegPlayer::setSource(const QString &value)
{
    m_source = value;
}
