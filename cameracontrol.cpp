#include<cameracontrol.h>

CameraControl::CameraControl(QObject*  parent):QObject(parent)
{

}

CameraControl::~CameraControl(){}

void CameraControl::registerQmlType()
{
    qmlRegisterType<CameraControl>("CameraControl",1,0,"CameraControl");
}

void CameraControl::send_cmd(int idx)
{
    QByteArray ba;
    ba.append(0xFF);
    ba.append(idx);
    var->send_serial(m_portId,ba);
}
