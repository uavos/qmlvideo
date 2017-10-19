import QtQuick 2.9
import QtQuick.Controls 2.2

import QtQuick.Layouts 1.3

import QtMultimedia 5.9
import FfmpegPlayer 10.0
import VideoController 10.0
import VideoCoords 11.0
import OsdItem 10.0
import OsdMenu 10.0
import CameraControl 1.0

Rectangle {
    id: root
    color: "black"
    border.width: 2
    property var cmd_off:          0x00
    property var cmd_right:        0x01
    property var cmd_down:         0x02
    property var cmd_play:         0x03
    property var cmd_up:           0x04
    property var cmd_left:         0x05
    property var cmd_menu:         0x06
    property var cmd_zoom_plus:    0x07
    property var cmd_zoom_min:     0x08
    property var cmd_shot:         0x09
    property var cmd_shot_ent:     0x0a
    property var cmd_on:           0x0b
    property var cmd_ok:           0x0c



    property var cmd_reload:       0x0E
    property var cmd_autofocus:    0x0F
    property var cmd_manualfocus:  0x10
    property var cmd_onepush:      0x11

    property bool btn_area: false;

    Item {
        id: config
        property var zoomInShortcut:    "Shift+W"
        property var zoomOutShortcut:   "Shift+E"
        property var snapshotShortcut:  "Shift+R"
        property var recdir:            "/home/gcu/BLA_Video/"
        property var photodir:          "/home/gcu/BLA_Foto/"
        property var reconnectTimeout:  20000
        property var cameras: [
            "rtsp://192.168.0.168/0",
            "rtsp://admin:12345@192.168.0.252:554/PSIA/streaming/channels/101",
            "rtsp://192.168.0.50:554/cam0_0",
        ]
        property var camerasAlias: [
            "ГОС-101 Ф",
            "ГОС-101 Т",
            "ГОС-101 Д",
        ]
    }
    Timer{
        interval:500; running: true; repeat: true
        onTriggered: if(user6.value){
                         player.snapshot();
                         user6.setValue(0);
                     }
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: config.zoomInShortcut
        onActivated: videoController.zoomIn()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: config.zoomOutShortcut
        onActivated: videoController.zoomOut()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: config.snapshotShortcut
        onActivated: player.snapshot()
    }

    ColumnLayout {
        id: toolbar
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            right: parent.right
            margins: 10
        }
        state: "visible"
        states: [
            State {
                name: "visible"
                AnchorChanges {
                    target: toolbar
                    anchors {
                        right: root.right
                        left: undefined
                    }
                }
            },
            State {
                name: "hidden"
                AnchorChanges {
                    target: toolbar
                    anchors {
                        left: root.right
                        right: undefined
                    }
                }
            }
        ]
        transitions: Transition {
            AnchorAnimation { duration: 300 }
        }
        width: 200
        ComboBox {
            Layout.fillWidth: true
            id: urlInput
            editable: true
            model: config.camerasAlias

            onAccepted: connect(true)
            onActivated: connect(false)
            function connect(custom)
            {
                var path;
                if(custom)
                {
                    path = editText
                }
                else
                {
                    path = config.cameras[currentIndex]
                }
                player.source = path
                player.play()
                videoController.ip = videoController.parseIpFromUrl(path)
                videoController.connect()
            }
        }

        Rectangle {
            Layout.minimumHeight: parent.height / 100 * 10
        }
        StackLayout
        {
            Layout.fillWidth: true
            currentIndex:urlInput.currentIndex
            ColumnLayout{
                ColumnLayout {
                    id: photoWidget
                    Layout.fillWidth: true
                    Button {
                        Layout.fillWidth: true
                        text: "Record"
                        checkable: true
                        onClicked: player.enableRecord(checked)
                    }
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
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: "<---"
                        Layout.preferredWidth: toolbar.width / 2
                        onPressed:{
                            for(var i=0;i<4;i++)
                              cameraControl.send_cmd(cmd_zoom_min)
                        }
                    }
                    Button {
                        text: "--->"
                        Layout.preferredWidth: toolbar.width / 2
                        onPressed:{
                            for(var i=0;i<4;i++)
                                 cameraControl.send_cmd(cmd_zoom_plus)
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: "ZOOM-"
                        Layout.preferredWidth: toolbar.width / 2
                        onPressed:{
                            cameraControl.send_cmd(cmd_zoom_min)
                        }
                                       onReleased: cameraControl.send_cmd(cmd_off)
                    }
                    Button {
                        text: "ZOOM+"
                        Layout.preferredWidth: toolbar.width / 2
                        onPressed:{
                            cameraControl.send_cmd(cmd_zoom_plus)
                        }
                        onReleased: cameraControl.send_cmd(cmd_off)
                    }
                }
            }
            ColumnLayout{
                id:tcamera
                Button {
                    Layout.fillWidth: true
                    text: "Record"
                    checkable: true
                    onClicked: player.enableRecord(checked)
                }
                Button {
                    Layout.fillWidth: true
                    text: "Snapshot"
                    onClicked: player.snapshot()
                }

            }
            ColumnLayout {
                id: videoWidget
                Layout.fillWidth: true
                Button {
                    Layout.fillWidth: true
                    text: "Record"
                    checkable: true
                    onClicked: player.enableRecord(checked)
                }
                Button {
                    Layout.fillWidth: true
                    text: "Snapshot"
                    onClicked: player.snapshot()
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        text: "Zoom in"
                        autoRepeat: true
                        onPressed: videoController.zoomIn()
                        Layout.preferredWidth: toolbar.width / 2
                    }
                    Button {
                        text: "Zoom out"
                        autoRepeat: true
                        onPressed: videoController.zoomOut();
                        Layout.preferredWidth: toolbar.width / 2
                    }
                }
            }


        }
    }
    Button {
        anchors {
            right: parent.right
            bottom: parent.bottom
        }
        flat: true
        text: toolbar.state == "visible" ? ">" : "<"
        onClicked: toolbar.state == "visible" ? toolbar.state = "hidden" : toolbar.state = "visible"
    }

    CameraControl{
        id: cameraControl
    }

    VideoCoords {
        id: videoCoords
    }
    VideoController {
        id: videoController
        ip: videoController.parseIpFromUrl(urlInput.currentText)
        zoomvar: "user2"
    }
    FfmpegPlayer {
        id: player
        source: config.cameras[0]
        recdir: config.recdir
        photodir: config.photodir
        reconnectTimeout: config.reconnectTimeout
        Component.onCompleted: play()
        osdMenu: osdMenu
    }

    VideoOutput {
        source: player
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            right: toolbar.left
        }
        Text {
            anchors.centerIn: parent
            text: player.stateText
            visible: player.playbackState != MediaPlayer.PlayingState
            color: "yellow"
            font.pixelSize: parent.height / 20
        }

        Rectangle {
            visible: player.playbackState == MediaPlayer.PlayingState
            anchors.centerIn: parent
            width: 1
            height: 100
            color: "red"
        }
        Rectangle {
            visible: player.playbackState == MediaPlayer.PlayingState
            anchors.centerIn: parent
            width: 100
            height: 1
            color: "red"
        }
        Timer {
            repeat: true
            interval: 1000
            running: true
            onTriggered: {
                currentTime.value = Qt.formatTime(new Date(), "hh:mm:ss")
                currentDate.value = Qt.formatDate(new Date(), "dd-MM-yyyy")
            }
        }

        OsdMenu {
            id: osdMenu
            items: [
                OsdItem { name: roll.name; value: roll.value; precision: 4 },
                OsdItem { name: pitch.name; value: pitch.value; precision: 4 },
                OsdItem { name: yaw.name; value: yaw.value; precision: 4 },
                OsdItem { name: cam_roll.name; value: cam_roll.value; precision: 4 },
                OsdItem { name: cam_pitch.name; value: cam_pitch.value; precision: 4 },
                OsdItem { name: cam_yaw.name; value: cam_yaw.value; precision: 4 },
                OsdItem { name: airspeed.name; value: airspeed.value; precision: 2 },
                OsdItem { name: "target"; value: videoCoords.targetString },
                OsdItem { name: "distance to target"; value: videoCoords.targetDistance; precision: 4 },
                OsdItem { id: currentDate; name: "date" },
                OsdItem { id: currentTime; name: "time" }
            ]
        }

        Column {
            id: osd
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 5
                topMargin: 5
            }

            Repeater {
                model: osdMenu.items
                Text {
                    color: "red"
                    text: modelData.name + ": " + modelData.value
                }
            }
        }
    }
}

