#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>

class VideoController: public QObject
{
    Q_OBJECT
public:
    static void registerQmlType();

    Q_PROPERTY(QString zoomvar READ getZoomVar WRITE setZoomVar)

    Q_INVOKABLE QString parseIpFromUrl(const QString &url);

    Q_PROPERTY(QString ip READ getIp WRITE setIp)
    VideoController();

    Q_INVOKABLE void connect();

    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();

    QString getIp() const;
    void setIp(const QString &ip);

    QString getZoomVar() const;
    void setZoomVar(const QString &zoomVar);

public slots:
    void zoomInStart();
    void zoomInStop();
    void zoomOutStart();
    void zoomOutStop();

private slots:
    void onZoomVarChanged();

private:
    QString m_ip;
    QString m_zoomVar;
    QTimer m_zoomInTimer;
    QTimer m_zoomOutTimer;
    QNetworkAccessManager m_network;
};

#endif // VIDEOCONTROLLER_H
