import QtQuick 2.12
//import Curve 1.0
//import CurveMovingPoint 1.0
import DisplayView 1.0
import PointView 1.0
import LineView 1.0

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
    onHighlightColorChanged: {
        pointView.color = highlightColor
        lineView.color = highlightColor
    }
    onHighlightSizeChanged: {
        pointView.size = highlightSize
        lineView.size = highlightSize
    }

    GraphCanvas {
        id: graphCanvas
        anchors.fill: parent
    }

    DisplayView {
        id: derivative2View
        objectName: "derivative2View"
        anchors.fill: parent
        visible: true
        layer.enabled: true
        layer.samples: 256
        color: "green"
        lineWidth: parameters.lineWidth
    }

    DisplayView {
        id: derivativeView
        objectName: "derivativeView"
        anchors.fill: parent
        visible: true
        layer.enabled: true
        layer.samples: 256
        color: "blue"
        lineWidth: parameters.lineWidth
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

    LineView {
        id: lineView
        objectName: "lineView"
        anchors.fill: parent
        layer.enabled: true
        layer.samples: 256
        color: parameters.highlightColor
        size: parameters.highlightSize
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
            functionController.startPinch()
        }
        onPinchUpdated: {
            functionController.pinch(pinch.scale)
        }
        MouseArea {
            anchors.fill: parent
            onWheel: {
                if (!parameters.exploreMode) {
                    //window.newGraph()
                    functionController.zoom(wheel.angleDelta.y)
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

    onWidthChanged: updateCanvas()
    onHeightChanged: updateCanvas()

    function updateCanvas() {
        window.stopAudio()
        graphCanvas.updateCanvas()
        functionController.viewDimensionsChanged()
    }

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
