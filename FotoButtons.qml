import QtQuick 2.9
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.3

ColumnLayout {
        id: photoWidget
        Layout.fillWidth: true
        Button {
            id:but
            Layout.fillWidth: true
            text: "Shot"
            onClicked: player.snapshot()
            onPressed: cameraControl.send_cmd(cmd_shot)
            onReleased: cameraControl.send_cmd(cmd_off)
            onHoveredChanged: if(btn_area){
                                 btn_area=false;
                              }else
                              {  btn_area=true;
                                 cameraControl.send_cmd(cmd_shot_ent)
                              }
        }
        GridLayout {
            Layout.fillWidth: true
            rows: 5
            columns: 5
            RoundButton {
                text: "Play"
                onPressed:cameraControl.send_cmd(cmd_play)
                onReleased:cameraControl.send_cmd(cmd_off)
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
                onPressed: cameraControl.send_cmd(cmd_up)
                onReleased: cameraControl.send_cmd(cmd_off)
            }
            Button {
                text: "Left"
                Layout.row: 2
                Layout.column: 0
                onPressed: cameraControl.send_cmd(cmd_left)
                onReleased: cameraControl.send_cmd(cmd_off)
            }
            Button {
                text: "Ok"
                Layout.row: 2
                Layout.column: 1
                onPressed: cameraControl.send_cmd(cmd_ok)
                onReleased:cameraControl.send_cmd(cmd_off)
            }
            Button {
                text: "Right"
                Layout.row: 2
                Layout.column: 2
                onPressed: cameraControl.send_cmd(cmd_right)
                onReleased: cameraControl.send_cmd(cmd_off)
            }
            Button {
                text: "Down"
                Layout.row: 3
                Layout.column: 1
                onPressed: cameraControl.send_cmd(cmd_down)
                onReleased: cameraControl.send_cmd(cmd_off)
            }
            RoundButton {
                text: "On"
                onPressed:cameraControl.send_cmd(cmd_on)
                onReleased: cameraControl.send_cmd(cmd_off)
                radius: width / 2
                Layout.row: 4
                Layout.column: 0
                Layout.minimumHeight: width
                Layout.fillWidth: true
            }
            RoundButton {
                text: "Menu"
                onPressed: cameraControl.send_cmd(cmd_menu)
                onReleased: cameraControl.send_cmd(cmd_off)
                radius: width / 2
                Layout.row: 4
                Layout.column: 2
                Layout.minimumHeight: width
                Layout.fillWidth: true
            }
        }
    }
