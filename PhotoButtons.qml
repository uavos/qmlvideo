import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import CameraControl 1.0

ColumnLayout {
    id: photoWidget
    Layout.fillWidth: true
    Button {
        Layout.fillWidth: true
        text: "Shot"
        onClicked: player.snapshot()
        onPressed: cameraControl.send_cmd(CameraControl.CMD_SHOT)
        onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
        onHoveredChanged: {
            if(hovered)
                cameraControl.send_cmd(CameraControl.CMD_SHOT_ENT)
        }
    }
    GridLayout {
        Layout.fillWidth: true
        rows: 5
        columns: 5
        RoundButton {
            text: "Play"
            onPressed: cameraControl.send_cmd(CameraControl.CMD_PLAY)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
            radius: width / 2
            Layout.row: 0
            Layout.column: 0
            Layout.minimumHeight: width
            Layout.fillWidth: true
        }
        Button {
            text: "Up"
            Layout.row: 1
            Layout.column: 1
            onPressed: cameraControl.send_cmd(CameraControl.CMD_UP)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
        }
        Button {
            text: "Left"
            Layout.row: 2
            Layout.column: 0
            onPressed: cameraControl.send_cmd(CameraControl.CMD_LEFT)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
        }
        Button {
            text: "Ok"
            Layout.row: 2
            Layout.column: 1
            onPressed: cameraControl.send_cmd(CameraControl.CMD_OK)
            onReleased:cameraControl.send_cmd(CameraControl.CMD_OFF)
        }
        Button {
            text: "Right"
            Layout.row: 2
            Layout.column: 2
            onPressed: cameraControl.send_cmd(CameraControl.CMD_RIGHT)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
        }
        Button {
            text: "Down"
            Layout.row: 3
            Layout.column: 1
            onPressed: cameraControl.send_cmd(CameraControl.CMD_DOWN)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
        }
        RoundButton {
            text: "On"
            onPressed: cameraControl.send_cmd(CameraControl.CMD_ON)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
            radius: width / 2
            Layout.row: 4
            Layout.column: 0
            Layout.minimumHeight: width
            Layout.fillWidth: true
        }
        RoundButton {
            text: "Menu"
            onPressed: cameraControl.send_cmd(CameraControl.CMD_MENU)
            onReleased: cameraControl.send_cmd(CameraControl.CMD_OFF)
            radius: width / 2
            Layout.row: 4
            Layout.column: 2
            Layout.minimumHeight: width
            Layout.fillWidth: true
        }
    }
}
