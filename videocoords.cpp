#include "videocoords.h"

#include <QtQml/qqml.h>
#include <qmath.h>
#include <QMandala.h>
#include <QQuaternion>

#define ALTITUDE_NAME   "altitude"
#define CAM_YAW_NAME    "cam_yaw"
#define CAM_ROLL_NAME   "cam_roll"
#define CAM_PITCH_NAME  "cam_pitch"
#define UAV_YAW_NAME    "yaw"
#define UAV_ROLL_NAME   "roll"
#define UAV_PITCH_NAME  "pitch"
#define UAV_LAT_NAME    "gps_lat"
#define UAV_LON_NAME    "gps_lon"

#define ALTITUDE        var->field(ALTITUDE_NAME)->value()
#define CAM_YAW         var->field(CAM_YAW_NAME)->value()
#define CAM_ROLL        var->field(CAM_ROLL_NAME)->value()
#define CAM_PITCH       var->field(CAM_PITCH_NAME)->value()
#define UAV_YAW         var->field(UAV_YAW_NAME)->value()
#define UAV_ROLL        var->field(UAV_ROLL_NAME)->value()
#define UAV_PITCH       var->field(UAV_PITCH_NAME)->value()
#define UAV_LAT         var->field(UAV_LAT_NAME)->value()
#define UAV_LON         var->field(UAV_LON_NAME)->value()

VideoCoords::VideoCoords()
{
    //    connect(var->field(ALTITUDE_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(UAV_YAW_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(CAM_ROLL_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(UAV_LAT_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(UAV_PITCH_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(CAM_PITCH_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    //    connect(var->field(UAV_LON_NAME), SIGNAL(changed()), this, SLOT(doTargetCoordsUpdate()));
    m_coordsUpdateTimer.setInterval(10);
    m_coordsUpdateTimer.setSingleShot(false);
    m_coordsUpdateTimer.start();
    connect(&m_coordsUpdateTimer, SIGNAL(timeout()), this, SLOT(doTargetCoordsUpdate()));
}

void VideoCoords::doTargetCoordsUpdate()
{
    emit targetLatChanged();
    emit targetLonChanged();
    emit targetStringChanged();
    emit targetDistanceChanged();
}

double VideoCoords::getTargetLat()
{
    QVector3D v(1.0, 0.0, 0.0);

    QQuaternion uavyq = QQuaternion::fromEulerAngles(0, 0, UAV_YAW);

    QQuaternion camrq = QQuaternion::fromEulerAngles(CAM_ROLL, 0, 0);
    QQuaternion campq = QQuaternion::fromEulerAngles(0, CAM_PITCH, 0);

    QQuaternion globalq = uavyq * camrq * campq;

    QVector3D rotated = globalq.rotatedVector(v);

    const double altitude = ALTITUDE;

    const double lat_meters = rotated.x() / rotated.z() * altitude;

    const double gps_point_lat = UAV_LAT + (lat_meters / 6371000.0) * (180.0 / M_PI);

    return gps_point_lat;
}

double VideoCoords::getTargetLon()
{
    QVector3D v(1.0, 0.0, 0.0);

    QQuaternion uavyq = QQuaternion::fromEulerAngles(0, 0, UAV_YAW);

    QQuaternion camrq = QQuaternion::fromEulerAngles(CAM_ROLL, 0, 0);
    QQuaternion campq = QQuaternion::fromEulerAngles(0, CAM_PITCH, 0);

    QQuaternion globalq = uavyq * camrq * campq;

    QVector3D rotated = globalq.rotatedVector(v);

    const double altitude = ALTITUDE;

    const double lon_meters = rotated.y() / rotated.z() * altitude;

    const double gps_point_lon = UAV_LON +
            (180.0 / M_PI) * lon_meters / (6371000.0 * qCos(qDegreesToRadians(UAV_LAT)));

    return gps_point_lon;
}

double VideoCoords::getTargetDistance()
{
    QVector3D v(1.0, 0.0, 0.0);

    QQuaternion uavyq = QQuaternion::fromEulerAngles(0, 0, UAV_YAW);

    QQuaternion camrq = QQuaternion::fromEulerAngles(CAM_ROLL, 0, 0);
    QQuaternion campq = QQuaternion::fromEulerAngles(0, CAM_PITCH, 0);

    QQuaternion globalq = uavyq * camrq * campq;

    QVector3D rotated = globalq.rotatedVector(v);

    const double altitude = ALTITUDE;

    const double lat_meters = rotated.x() / rotated.z() * altitude;
    const double lon_meters = rotated.y() / rotated.z() * altitude;
    QVector3D full(lat_meters, lon_meters, altitude);

    return full.length();
}

QString VideoCoords::getTargetString()
{
    return coordsToString(getTargetLat(), getTargetLon());
}

void VideoCoords::registerQmlType()
{
    qmlRegisterType<VideoCoords>("VideoCoords", 11, 0, "VideoCoords");
}

QString VideoCoords::coordsToString(double lat, double lon)
{
    if(lookToGround())
    {
        QString coordsString = QString("%1;%2")
                .arg(mandala->latToString(lat))
                .arg(mandala->lonToString(lon));
        return coordsString;
    }
    else
        return "No ground target";
}

std::pair<double, double> VideoCoords::rotate(const std::pair<double, double> &v, double angle)
{
    double theta = qDegreesToRadians(-angle);

    double cs = qCos(theta);
    double sn = qSin(theta);

    std::pair<double, double> result;
    result.first = v.first * cs - v.second * sn;
    result.second = v.first * sn + v.second * cs;
    return result;
}

bool VideoCoords::lookToGround()
{
    QVector3D v(1.0, 0.0, 0.0);

    QQuaternion uavyq = QQuaternion::fromEulerAngles(0, 0, UAV_YAW);

    QQuaternion camrq = QQuaternion::fromEulerAngles(CAM_ROLL, 0, 0);
    QQuaternion campq = QQuaternion::fromEulerAngles(0, CAM_PITCH, 0);

    QQuaternion globalq = uavyq * camrq * campq;

    QVector3D rotated = globalq.rotatedVector(v);

    return rotated.z() > 0;
}
