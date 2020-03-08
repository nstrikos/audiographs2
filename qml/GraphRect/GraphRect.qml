import QtQuick 2.12
//import Curve 1.0
//import CurveMovingPoint 1.0
import DisplayView 1.0
import PointView 1.0

import "../BeautityRect"

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

    onCurveColorChanged: displayView.color = curveColor
    onCurveWidthChanged: displayView.lineWidth = curveWidth
    onHighlightColorChanged: pointView.color = highlightColor
    onHighlightSizeChanged: pointView.size = highlightSize

    GraphCanvas {
        id: graphCanvas
        anchors.fill: parent
    }

    DisplayView {
        id: displayView
        objectName: "displayView"
        anchors.fill: parent
        visible: true
        layer.enabled: true
        layer.samples: 256
        color: parameters.lineColor
        lineWidth: parameters.lineWidth
    }

    PointView {
        id: pointView
        objectName: "pointView"
        anchors.fill: parent
        layer.enabled: true
        layer.samples: 256
        color: parameters.highlightColor
        size: parameters.highlightSize
    }

    PinchArea {
        anchors.fill: parent
        onPinchStarted: {
            controlsRect.stopAudio()
            functionController.startPinch()
        }
        onPinchUpdated: {
            controlsRect.stopAudio()
            functionController.pinch(pinch.scale)
        }
        MouseArea {
            anchors.fill: parent
            onWheel: {
                if (!parameters.exploreMode)
                    functionController.zoom(wheel.angleDelta.y)
                else {
                    if (wheel.angleDelta.y > 0)
                        functionController.previousPoint()
                    else
                        functionController.nextPoint()
                }
            }
            onPressedChanged: {
                if (!parameters.exploreMode) {
                    if (pressed)
                        functionController.startDrag(mouseX, mouseY)
                } else {
                    if (!pressed)
                        functionController.stopAudio()
                }
            }
            onPositionChanged: {
                if (!parameters.exploreMode) {
                    if (pressed)
                        functionController.drag(mouseX, mouseY, width, height)
                } else {
                    if (pressed)
                        functionController.mousePoint(mouseX)
                }
            }
        }
    }

    function updateCanvas() {
        graphCanvas.updateCanvas()
        functionController.updateView()
    }

    onWidthChanged: updateCanvas()
    onHeightChanged: updateCanvas()

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