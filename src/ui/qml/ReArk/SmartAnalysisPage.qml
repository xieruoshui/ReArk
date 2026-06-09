import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Controls.Material
import QtQuick.Layouts

Rectangle {
    id: root

    readonly property bool darkTheme: Material.theme === Material.Dark
    readonly property color pageColor: darkTheme ? "#dfe8f4" : "#e8eef7"
    readonly property color panelColor: "#fbfcff"
    readonly property color primaryTextColor: "#101828"
    readonly property color secondaryTextColor: "#667085"
    readonly property color borderColor: "#cfd8e6"
    readonly property color iconColor: "#2b3a55"
    readonly property color accentColor: "#5b82f1"
    readonly property color accentHoverColor: "#4c73df"

    color: pageColor

    Button {
        id: newChatButton

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 16
        anchors.rightMargin: 22
        width: Math.max(106, newChatContent.implicitWidth + 30)
        height: 36
        padding: 0
        hoverEnabled: true

        background: Rectangle {
            radius: 18
            color: newChatButton.hovered ? "#ffffff" : "#f7f9fd"
            border.width: 1
            border.color: root.borderColor
        }

        contentItem: Row {
            id: newChatContent

            anchors.centerIn: parent
            spacing: 8

            Text {
                text: "\uE710"
                color: root.iconColor
                font.family: "Segoe MDL2 Assets"
                font.pixelSize: 11
                anchors.verticalCenter: parent.verticalCenter
                renderType: Text.NativeRendering
            }

            Text {
                text: qsTr("New Chat")
                color: root.primaryTextColor
                font.pixelSize: 13
                font.weight: Font.DemiBold
                anchors.verticalCenter: parent.verticalCenter
                renderType: Text.NativeRendering
            }
        }
    }

    ColumnLayout {
        width: Math.min(parent.width - 96, 930)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -26
        spacing: 16

        Label {
            Layout.fillWidth: true
            text: qsTr("What do you want to protect?")
            color: root.primaryTextColor
            font.pixelSize: 32
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            radius: 8
            color: root.panelColor
            border.width: 1
            border.color: root.borderColor
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowBlur: 0.6
                shadowOpacity: 0.18
                shadowVerticalOffset: 4
            }

            TextArea {
                id: promptInput

                anchors.left: parent.left
                anchors.right: sendButton.left
                anchors.top: parent.top
                anchors.bottom: toolRow.top
                anchors.leftMargin: 18
                anchors.rightMargin: 16
                anchors.topMargin: 14
                anchors.bottomMargin: 8
                background: null
                wrapMode: TextEdit.Wrap
                placeholderText: qsTr("Ask anything about app protection")
                placeholderTextColor: root.secondaryTextColor
                color: root.primaryTextColor
                selectedTextColor: "#ffffff"
                selectionColor: root.accentColor
                font.pixelSize: 13
                padding: 0
            }

            Row {
                id: toolRow

                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 22
                anchors.bottomMargin: 20
                spacing: 18

                Text {
                    text: "\uE16C"
                    color: root.iconColor
                    font.family: "Segoe MDL2 Assets"
                    font.pixelSize: 15
                    renderType: Text.NativeRendering
                }

                Text {
                    text: "\u25C7"
                    color: root.iconColor
                    font.family: "Segoe UI Symbol"
                    font.pixelSize: 16
                    font.bold: true
                    renderType: Text.NativeRendering
                }
            }

            Button {
                id: sendButton

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 16
                anchors.bottomMargin: 14
                width: 40
                height: 40
                padding: 0
                hoverEnabled: true

                background: Rectangle {
                    radius: width / 2
                    color: sendButton.hovered ? root.accentHoverColor : root.accentColor
                }

                contentItem: Text {
                    text: "\uE74A"
                    color: "#ffffff"
                    font.family: "Segoe MDL2 Assets"
                    font.pixelSize: 16
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    renderType: Text.NativeRendering
                }
            }
        }
    }
}
