import QtQuick 2.12
import Curve 1.0
import CurveMovingPoint 1.0

Rectangle {
    id: graphRect
    z: 10
    color: "white"

    layer.enabled: true
    layer.samples: 256

    property var graphCanvas: graphCanvas
    property color curveColor: parameters.lineColor
    property var curveWidth: parameters.lineWidth
    property color highlightColor: parameters.highlightColor
    property var highlightSize: parameters.highlightSize

    property alias curveMovingPoint: curveMovingPoint

    GraphCanvas {
        id: graphCanvas
        anchors.fill: parent
    }

    Curve {
        id: curve
        anchors.fill: parent
        visible: false
        layer.enabled: true
        layer.samples: 256
        color: parameters.lineColor
        lineWidth: parameters.lineWidth
    }

    CurveMovingPoint {
        id: curveMovingPoint
        anchors.fill: parent
        layer.enabled: true
        layer.samples: 256
        color: parameters.highlightColor
        size: parameters.highlightSize
    }

    onCurveColorChanged: curve.color = curveColor
    onCurveWidthChanged: curve.lineWidth = curveWidth
    onHighlightColorChanged: curveMovingPoint.color = highlightColor
    onHighlightSizeChanged: curveMovingPoint.size = highlightSize

    PinchArea {
        anchors.fill: parent
        onPinchStarted: controlsRect.startPinch()
        onPinchUpdated: controlsRect.handlePinch(pinch.scale)
        onPinchFinished: controlsRect.pinchFinished()
        MouseArea {
            anchors.fill: parent
            onWheel: controlsRect.handleZoom(wheel.angleDelta.y)

            onPressedChanged: {
                if (pressed)
                    controlsRect.startDrag(mouseX, mouseY)
            }
            onPositionChanged: {
                if (pressed)
                    controlsRect.handleDrag(mouseX, mouseY)
            }
        }
    }

    function updateCanvas() {
        graphCanvas.updateCanvas()
        curve.draw(myfunction)
        curve.visible = true
    }

    function startMovingPoint() {
        curveMovingPoint.drawPoint(myfunction, parameters.duration)
    }

    function stopMovingPoint() {
        curveMovingPoint.stopPoint()
    }

    onWidthChanged: controlsRect.calculate()
    onHeightChanged: controlsRect.calculate()

    BeautifyGraphRect {

    }

    BeautifyGraphRect {
        anchors.top: parent.top
        anchors.bottom: undefined
        height: 8
        visible: (settingsRect.height > 0) && (!anchorToLeft)

        gradient: Gradient {
            GradientStop {
                color: Qt.rgba(0, 0, 0, 0.4)
                position: 0
            }
            GradientStop {
                color: Qt.rgba(0, 0, 0, 0.15)
                position: 0.5
            }
            GradientStop {
                color: Qt.rgba(0, 0, 0, 0)
                position: 1
            }
        }
    }

    BeautifyRect {
        visible: (settingsRect.width > 0)
    }
}