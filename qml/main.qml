import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.settings 1.1
import QtQuick.Dialogs 1.1

import QtQml.StateMachine 1.0 as DSM

import "ControlsRect"
import "GraphRect"
import "SettingsRect"

Window {
    id: window
    visible: true

    //on android setting width and height results in
    //not showing correctly the application
    //    width: Qt.platform.os === "android" ? 320 : 1024//: Screen.width
    //    height: Qt.platform.os === "android" ? 350 : 768//Screen.height
    minimumWidth: 320
    minimumHeight: 320
    title: qsTr("Audiographs")

    property bool anchorToLeft: undefined

    property alias settingsRect: settingsRect

    signal evaluate()
    signal playPressed()
    signal newGraph()
    signal error()
    signal init()
    signal stopAudio()
    signal explore()
    signal interestingPoint()
    signal interestingPointStopped()

    Settings {
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }

    ControlsRect {
        id: controlsRect
    }

    ControlsButton {
        id: controlsButton
    }

    GraphRect {
        id: graphRect
    }

    SettingsButton {
        id: settingsButton
    }

    SettingsRect {
        id: settingsRect
    }

    AnchorChangeState {
        id: anchorChangeState
    }

    Component.onCompleted: {
        setAnchor()
        if (anchorToLeft)
            anchorChangeState.state = 'state1'
        else
            anchorChangeState.state = 'state4'

        controlsRect.textInput.forceActiveFocus()
    }

    onWidthChanged: setAnchor()
    onHeightChanged: setAnchor()

    function setAnchor() {
        if (width >= height)
            anchorToLeft = true
        else
            anchorToLeft = false

    }
    onAnchorToLeftChanged: anchorChangeState.anchorChanged()

    function setColor() {
        controlsRect.color = !parameters.invertTheme ? "white" : "black"
        controlsRect.fontColor = parameters.invertTheme ? "white" : "black"
    }

    Shortcuts {
        id: shortcuts
    }

    StateMachine {
    }

//    Connections {
//        target: functionController
//        onUpdateFinished: newGraph()
//        onNewInputValues: controlsRect.newInputValues(minX, maxX, minY, maxY)
//        onMovingPointFinished: stopAudio()
//        onInterestingPointStopped: interestingPointStopped()
//        onError: error()
//    }

    Connections {
        target: functionController
        function onUpdateFinished() {
            newGraph()
        }
    }

    Connections {
        target: functionController
        function onNewInputValues(minX, maxX, minY, maxY) {
            controlsRect.newInputValues(minX, maxX, minY, maxY)
        }
    }

    Connections {
        target: functionController
        function onMovingPointFinished() {
            stopAudio()
        }
    }

    Connections {
        target: functionController
        function onError() {
            error()
        }
    }

    function showError(errorString) {
        messageDialog.title = errorString
        messageDialog.visible = true
        timer.start()
    }

    MessageDialog {
        id: messageDialog
        title: "May I have your attention please"
        text: title//"It's so cool that you are using Qt Quick."
        onAccepted: visible = false
        Component.onCompleted: visible = false
//        Accessible.name: qsTr("Error")
//        Accessible.description: qsTr("Error")
    }

    Timer {
        id: timer
        interval: 3000
        onTriggered: {
            messageDialog.visible = false
            window.init()
        }
    }
}
