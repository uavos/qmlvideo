import QtQuick 2.9
import QtQuick.Controls 2.2

import QtMultimedia 5.0
import FfmpegPlayer 10.0
import VideoController 10.0
import VideoCoords 11.0
import OsdItem 10.0
import OsdMenu 10.0

Rectangle {
    id: window
    property var cameraLabels: [
        "Camera1",
        "Camera2"
    ]
    property var cameraUrls: [
        "rtsp://admin:bagira11@192.168.0.50:554/PSIA/streaming/channels/101",
        "rtsp://admin:bagira11@192.168.0.50:554/PSIA/streaming/channels/201"
    ]
    property int diagonal: Math.sqrt(Math.pow(height,2) + Math.pow(width,2))
    property string currentUrl: cameraUrls[0];
    color: "black"
    border.width: 2

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Shift+W"
        onActivated: videoController.zoomIn()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Shift+E"
        onActivated: videoController.zoomOut()
    }

    Shortcut {
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Shift+R"
        onActivated: player.snapshot()
    }

    Rectangle {
        id: toolbar
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
        width: 200
        color: "black"

        Column {
            id: videoChooser
            anchors {
                top: parent.top
                bottom: parent.verticalCenter
                left: parent.left
                right: parent.right
            }
            spacing: 10

            Repeater {
                id: cameraRepeater
                model: window.cameraUrls
                Button {
                    anchors {
                        left: parent.left
                        right: parent.right
                        rightMargin: 20
                    }
                    checkable: true
                    text: window.cameraLabels[index]
                    onClicked: {
                        for(var i = 0; i < cameraRepeater.count; i++)
                            cameraRepeater.itemAt(i).checked = false;
                        checked = true;
                        player.source = cameraUrls[index]
                        player.play()
                        videoController.ip = videoController.parseIpFromUrl(cameraUrls[index])
                        videoController.connect()
                    }
                }
            }
        }

        Rectangle {
            id: videoWidget
            color: "black"
            anchors {
                top: videoChooser.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            Rectangle {
                property bool checked: false
                id: buttonRecVideo
                anchors {
                    right : parent.right
                    rightMargin : 20
                    verticalCenter: parent.verticalCenter
                }
                width: parent.width
                height: width / 4
                gradient: Gradient {
                    GradientStop { position: 1.0; color: "#727271" }
                    GradientStop { position: 0.0; color: "#2B2A29" }
                }
                radius: width / 2
                Text {
                    text: qsTr("Record")
                    anchors.centerIn: parent
                    font {
                        pixelSize: 18
                        bold: buttonRecVideo.checked
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        buttonRecVideo.checked = !buttonRecVideo.checked
                        player.enableRecord(buttonRecVideo.checked)
                    }
                }
            }

            Rectangle {
                anchors {
                    bottom: buttonRecVideo.top
                    bottomMargin: 70
                    horizontalCenter: buttonRecVideo.horizontalCenter
                }
                width: parent.width
                height: width / 4
                gradient: Gradient {
                    GradientStop { position: 0.00; color: "red"; }
                    GradientStop { position: 1.00; color: "darkred"; }
                }
                radius:  height / 2
                border {
                    width: 3
                    color: "#534F50"
                }

                Text {
                    id: buttonSnapshotLabel
                    text: qsTr("Snapshot")
                    anchors.centerIn: parent
                    font.pixelSize: 16
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: {
                        parent.border.color = "gray"
                        user5.setValue(11)
                    }
                    onExited: {
                        parent.border.color = "#534F50"
                        user5.setValue(0)
                    }
                    onPressed:  buttonSnapshotLabel.color = "white"
                    onReleased: buttonSnapshotLabel.color = "black"
                    onClicked:  player.snapshot()
                }
            }

            Rectangle {
                id: buttonZoomVideo
                anchors {
                    top: buttonRecVideo.bottom
                    topMargin: 70
                    horizontalCenter: buttonRecVideo.horizontalCenter
                }
                width: parent.width
                height: width / 4
                gradient: Gradient {
                    GradientStop { position: 0.00; color: "#434242"; }
                    GradientStop { position: 1.00; color: "#8f8d8d"; }
                }
                radius: height / 2
                border {
                    width: 3
                    color: "#534F50"
                }

                Rectangle {
                    anchors {
                        top: parent.top
                        left: buttonZoomVideo.left
                    }
                    width: parent.width / 2
                    height: buttonZoomVideo.height
                    color: "#00000000"
                    Text {
                        id: buttonZoomOutVideoLabel
                        text: qsTr("- Zoom")
                        anchors.centerIn: parent
                        font.pixelSize: 18
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        Timer {
                            id: zoomOutTimer
                            interval: 10
                            repeat: true
                            onTriggered: videoController.zoomOut()
                        }

                        onPressed: {
                            buttonZoomOutVideoLabel.color = "white";
                            videoController.zoomOut()
                            zoomOutTimer.start()
                        }
                        onReleased: {
                            buttonZoomOutVideoLabel.color = "black";
                            zoomOutTimer.stop()
                        }
                    }
                }

                Rectangle {
                    anchors {
                        top: parent.top
                        right: buttonZoomVideo.right
                    }
                    width: buttonZoomVideo.width / 2
                    height: buttonZoomVideo.height
                    color: "#00000000"
                    Text {
                        id: buttonZoomInVideoLabel
                        text: qsTr("Zoom +")
                        anchors.centerIn: parent
                        font.pixelSize: 18
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true

                        Timer {
                            id: zoomInTimer
                            interval: 10
                            repeat: true
                            onTriggered: videoController.zoomIn()
                        }

                        onPressed: {
                            buttonZoomInVideoLabel.color = "white";
                            videoController.zoomIn();
                            zoomInTimer.start()
                        }
                        onReleased: {
                            buttonZoomInVideoLabel.color = "black";
                            zoomInTimer.stop();
                        }
                    }
                }
            }
        }

        CheckBox {
            id: videoStabilizationCheckbox
            visible: player.isStabSupport
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
            checked: player.enableStabilizier
            text: "Stabilization"
            onCheckedChanged: {
                player.enableStabilizier = checked;
            }
        }
    }

    VideoCoords {
        id: videoCoords
    }
    VideoController {
        id: videoController
        ip: videoController.parseIpFromUrl(window.currentUrl)
        zoomvar: "user2"
    }
    FfmpegPlayer {
        id: player
        osdMenu: osdMenu
        source: window.currentUrl
        //        recdir: "/home/pavel/test/"
        reconnectTimeout: 20000
        Component.onCompleted: play()
    }
    VideoOutput {
        id: videoOutput
        source: player
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            right: toolbar.left
        }
        Rectangle {
            anchors.centerIn: parent
            width: 1
            height: 100
            color: "red"
        }
        Rectangle {
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
                OsdItem { name: roll.name; value: roll.value; precision: 2 },
                OsdItem { name: pitch.name; value: pitch.value; precision: 3 },
                OsdItem { name: yaw.name; value: yaw.value; precision: 4 },
                OsdItem { name: airspeed.name; value: airspeed.value; precision: 2 },
                OsdItem { name: roll.name; value: roll.value; precision: 2 },
                OsdItem { name: "target"; value: videoCoords.targetString },
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
