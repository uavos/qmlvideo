#include "cameracontrol.h"

CameraControl::CameraControl(QObject *parent):
    QObject(parent),
    m_portId(10),
    m_behaviour(BEHAVIOUR_OLD)
{
}

void CameraControl::registerQmlType()
{
    qmlRegisterType<CameraControl>("CameraControl", 1, 0, "CameraControl");
}

void CameraControl::send_cmd(CameraCommand command)
{
    QByteArray ba;
    ba.append(0xFF);
    ba.append(uint8_t(command));
    if((command == CMD_ZOOM_IN || command == CMD_ZOOM_OUT) && m_behaviour == BEHAVIOUR_NEW)
        for(int i = 0; i < 4; i++)
            var->send_serial(m_portId, ba);
    else
        var->send_serial(m_portId, ba);
}

int CameraControl::getPortId() const
{
    return m_portId;
}

void CameraControl::setPortId(int portId)
{
    m_portId = portId;
}

int CameraControl::getBehaviour() const
{
    return m_behaviour;
}

void CameraControl::setBehaviour(int behaviour)
{
    m_behaviour = behaviour;
}
