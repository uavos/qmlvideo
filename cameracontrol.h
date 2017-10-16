#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include<QObject>
#include<QMandala.h>
#include<QtQml/qqml.h>    //check
class CameraControl:public QObject{
    Q_OBJECT
public:
    CameraControl(QObject *parent =0);
    ~CameraControl();
    Q_INVOKABLE void send_cmd(int idx);
    static void registerQmlType();

private:
    static const int m_portId = 10;  //def
};

#endif // CAMERACONTROL_H
