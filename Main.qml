import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: root
    visible: true
    visibility: Window.FullScreen
    title: qsTr("mandelbrot-qml")
    color: "dark gray"

    property double zoom: 0.004
    property double offsetX: 0
    property double offsetY: 0

    property int imageW: mb.width()
    property int imageH: mb.height()
    property double imageZ: 0.004

    property int oldX: 0
    property int oldY: 0

    function zoomIn() {
        zoom = zoom / 1.5
        mb.update(zoom, offsetX, offsetY)
        mandelbrot.source = ""
        mandelbrot.source = "image://mb_provider"
    }

    function zoomOut() {
        zoom = zoom * 1.5
        mb.update(zoom, offsetX, offsetY)
        mandelbrot.source = ""
        mandelbrot.source = "image://mb_provider"
    }

    Image {
        id: mandelbrot
        x: 0
        y: 0
        anchors.fill: parent
        source: "image://mb_provider"

        MouseArea {
            anchors.fill: parent
            onPositionChanged: {
                var temp_offsetX = offsetX + ((oldX - mouseX) / imageW) * 5
                        * (zoom / imageZ)
                var temp_offsetY = offsetY + ((oldY - mouseY) / imageH) * 5
                        * (imageH / imageW) * (zoom / imageZ)
                mb.update(zoom, temp_offsetX, temp_offsetY)
                mandelbrot.source = ""
                mandelbrot.source = "image://mb_provider"
            }
            onPressed: {
                oldX = mouseX
                oldY = mouseY
            }
            onReleased: {
                offsetX = offsetX + ((oldX - mouseX) / imageW) * 5
                        * (zoom / imageZ)
                offsetY = offsetY + ((oldY - mouseY) / imageH) * 5
                        * (imageH / imageW) * (zoom / imageZ)
                mb.update(zoom, offsetX, offsetY)
                mandelbrot.source = ""
                mandelbrot.source = "image://mb_provider"
            }
            onWheel: {
                var y = wheel.angleDelta.y
                if (y > 0) {
                    zoomIn()
                } else {
                    zoomOut()
                }
                mb.update(zoom, offsetX, offsetY)
                mandelbrot.source = ""
                mandelbrot.source = "image://mb_provider"

            }
        }
    }

    Button {
        x: 10
        y: 10
        width: 40
        height: 40
        Text {
            anchors.fill: parent
            text: "↖"
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        onClicked: {
            root.close()
        }
    }

    Button {
        x: imageW - width - 10
        y: 10
        z: 10
        width: 40
        height: 40
        Text {
            anchors.fill: parent
            text: "+"
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            zoomIn()
        }
    }

    Button {
        x: imageW - width - 10
        y: 60
        z: 10
        width: 40
        height: 40
        Text {
            anchors.fill: parent
            text: "–"
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            zoomOut()
        }
    }
}
