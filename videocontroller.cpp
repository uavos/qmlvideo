#include "videocontroller.h"

#include <QtQml/qqml.h>
#include <QRegularExpression>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMandala.h>
#include <iostream>

void VideoController::registerQmlType()
{
    qmlRegisterType<VideoController>("VideoController", 10, 0, "VideoController");
}

QString VideoController::parseIpFromUrl(const QString &url)
{
    QRegularExpression regex("(\\d+\\.\\d+\\.\\d+\\.\\d+)");
    return regex.match(url).captured(0);
}

VideoController::VideoController()
{
    QObject::connect(&m_zoomInTimer, SIGNAL(timeout()), this, SLOT(zoomInStop()));
    QObject::connect(&m_zoomOutTimer, SIGNAL(timeout()), this, SLOT(zoomOutStop()));
    m_zoomInTimer.setInterval(500);
    m_zoomInTimer.setSingleShot(true);
    m_zoomOutTimer.setInterval(500);
    m_zoomOutTimer.setSingleShot(true);
}

void VideoController::connect()
{
    const QString INIT_URL = "http://%1/asp-get/fwpresetget.asp?FwModId=0&PortId=0";
    QNetworkRequest initRequest(QUrl(INIT_URL.arg(m_ip)));
    m_network.get(initRequest)->deleteLater();
}

void VideoController::zoomInStart()
{
    const QString ZOOM_IN_PR_URL = "http://%1/goform/app/FwPtzCtr?FwModId=0&PortId=0&PtzCode=0x10B&PtzParm=10&FwCgiVer=0x0001";
    QNetworkRequest request(QUrl(ZOOM_IN_PR_URL.arg(m_ip)));
    m_network.get(request)->deleteLater();
}

void VideoController::zoomInStop()
{
    const QString ZOOM_IN_R_URL = "http://%1/goform/app/FwPtzCtr?FwModId=0&PortId=0&PtzCode=0x20B&PtzParm=10&FwCgiVer=0x0001";
    QNetworkRequest request(QUrl(ZOOM_IN_R_URL.arg(m_ip)));
    m_network.get(request)->deleteLater();
}

void VideoController::zoomOutStart()
{
    const QString ZOOM_OUT_PR_URL = "http://%1/goform/app/FwPtzCtr?FwModId=0&PortId=0&PtzCode=0x10C&PtzParm=10&FwCgiVer=0x0001";
    QNetworkRequest request(QUrl(ZOOM_OUT_PR_URL.arg(m_ip)));
    m_network.get(request)->deleteLater();
}

void VideoController::zoomOutStop()
{
    const QString ZOOM_OUT_R_URL = "http://%1/goform/app/FwPtzCtr?FwModId=0&PortId=0&PtzCode=0x20C&PtzParm=10&FwCgiVer=0x0001";
    QNetworkRequest request(QUrl(ZOOM_OUT_R_URL.arg(m_ip)));
    m_network.get(request)->deleteLater();
}

void VideoController::zoomIn()
{
    zoomInStart();
    m_zoomInTimer.start();
}

void VideoController::zoomOut()
{
    zoomOutStart();
    m_zoomOutTimer.start();
}

QString VideoController::getIp() const
{
    return m_ip;
}

void VideoController::setIp(const QString &ip)
{
    m_ip = ip;
}

QString VideoController::getZoomVar() const
{
    return m_zoomVar;
}

void VideoController::setZoomVar(const QString &zoomVar)
{
    if(var->names.contains(zoomVar))
    {
        disconnect(var->field(m_zoomVar), SIGNAL(changed()), this, SLOT(onZoomVarChanged()));
        m_zoomVar = zoomVar;
        QObject::connect(var->field(m_zoomVar), SIGNAL(changed()), this, SLOT(onZoomVarChanged()));

    }
    else
        std::cout << "var " << zoomVar.toStdString() << " doesn't exist" << std::endl;
}

void VideoController::onZoomVarChanged()
{
    if(var->names.contains(m_zoomVar))
    {
        QMandalaField *field = var->field(m_zoomVar);
        if(field->value() > 0)
            zoomIn();
        else if(field->value() < 0)
            zoomOut();
    }
    else
        std::cout << "var " << m_zoomVar.toStdString() << " doesn't exist" << std::endl;
}

