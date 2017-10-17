#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QObject>
#include <QMandala.h>
#include <QtQml/qqml.h>

class CameraControl: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int portId READ getPortId WRITE setPortId)
    Q_PROPERTY(int behaviour READ getBehaviour WRITE setBehaviour)
public:
    enum CameraCommand
    {
        CMD_OFF             = 0x00,
        CMD_RIGHT           = 0x01,
        CMD_DOWN            = 0x02,
        CMD_PLAY            = 0x03,
        CMD_UP              = 0x04,
        CMD_LEFT            = 0x05,
        CMD_MENU            = 0x06,
        CMD_ZOOM_IN       = 0x07,
        CMD_ZOOM_OUT        = 0x08,
        CMD_SHOT            = 0x09,
        CMD_SHOT_ENT        = 0x0A,
        CMD_ON              = 0x0B,
        CMD_OK              = 0x0C,
        CMD_RELOAD          = 0x0E,
        CMD_FOCUS_AUTO      = 0x0F,
        CMD_FOCUS_MANUAL    = 0x10,
        CMD_ONEPUSH         = 0x11
    };
    Q_ENUMS(CameraCommand)
    enum Behaviour
    {
        BEHAVIOUR_OLD,
        BEHAVIOUR_NEW
    };
    Q_ENUMS(Behaviour)
    static void registerQmlType();

    CameraControl(QObject *parent = 0);

    Q_INVOKABLE void send_cmd(CameraCommand command);

    int getPortId() const;
    void setPortId(int portId);

    int getBehaviour() const;
    void setBehaviour(int behaviour);

private:
    int m_portId = 10;
    int m_behaviour;
};

#endif // CAMERACONTROL_H
