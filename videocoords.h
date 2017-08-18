#ifndef VIDEOCOORDS_H
#define VIDEOCOORDS_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "osdmenu.h"

class VideoCoords: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double targetLat READ getTargetLat NOTIFY targetLatChanged)
    Q_PROPERTY(double targetLon READ getTargetLon NOTIFY targetLonChanged)
    Q_PROPERTY(QString targetString READ getTargetString NOTIFY targetStringChanged)
    Q_PROPERTY(double targetDistance READ getTargetDistance NOTIFY targetDistanceChanged)
public:
    static void registerQmlType();
    double getTargetLat();
    double getTargetLon();
    double getTargetDistance();
    QString getTargetString();

    VideoCoords();

private slots:
    void doTargetCoordsUpdate();

private:
    double m_pitchCorrection;
    QTimer m_coordsUpdateTimer;
    QString coordsToString(double lat, double lon);
    std::pair<double, double> rotate(const std::pair<double, double> &v, double angle);
    bool lookToGround();

signals:
    void targetLatChanged();
    void targetLonChanged();
    void targetStringChanged();
    void targetDistanceChanged();
};

#endif // VIDEOCOORDS_H
