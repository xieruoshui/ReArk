import QtQuick
import QtQuick.Controls

Item {
    id: root

    property string markdown: ""
    property bool markdownEnabled: true
    property bool darkTheme: true
    property color textColor: "#eef5ff"
    property color accentColor: "#6f8cff"
    property int textPixelSize: 13
    property string emptyText: ""

    readonly property string displayText: markdown.length > 0 ? markdown : emptyText
    property string renderedText: displayText

    implicitWidth: Math.max(1, body.implicitWidth)
    implicitHeight: Math.max(1, body.implicitHeight)

    function renderNow() {
        if (markdownEnabled && typeof markdownRenderer !== "undefined") {
            renderedText = markdownRenderer.render(displayText, darkTheme)
        } else {
            renderedText = displayText
        }
    }

    function scheduleRender() {
        if (markdownEnabled && typeof markdownRenderer !== "undefined") {
            renderTimer.restart()
        } else {
            renderTimer.stop()
            renderNow()
        }
    }

    onDisplayTextChanged: scheduleRender()
    onMarkdownEnabledChanged: scheduleRender()
    onDarkThemeChanged: scheduleRender()
    Component.onCompleted: renderNow()

    Timer {
        id: renderTimer

        interval: 60
        repeat: false
        onTriggered: root.renderNow()
    }

    TextEdit {
        id: body

        width: root.width > 0 ? root.width : implicitWidth
        readOnly: true
        selectByMouse: true
        wrapMode: TextEdit.Wrap
        textFormat: root.markdownEnabled ? TextEdit.RichText : TextEdit.PlainText
        text: root.renderedText
        color: root.textColor
        selectedTextColor: "#ffffff"
        selectionColor: root.accentColor
        font.pixelSize: root.textPixelSize
        renderType: Text.NativeRendering
        opacity: root.displayText.length > 0 ? 1.0 : 0.66

        onLinkActivated: function(link) {
            Qt.openUrlExternally(link)
        }
    }
}
