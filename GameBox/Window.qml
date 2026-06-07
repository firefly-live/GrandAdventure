import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: mainMenu
    width: 900
    height: 600
    visible: true
    title: "游戏盒子"

    // 背景
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a2e" }
            GradientStop { position: 1.0; color: "#16213e" }
        }
    }

    // 标题与积分
    ColumnLayout {
        anchors.top: parent.top
        anchors.topMargin: 40
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 10

        Text {
            text: "🎮 游戏盒子"
            font.pixelSize: 32
            font.bold: true
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            property int total: (typeof gameManager !== "undefined" && gameManager) ? gameManager.totalScore : 0
            text: "总积分: " + total
            font.pixelSize: 24
            color: "white"
            Layout.alignment: Qt.AlignHCenter
        }
    }

    // 游戏选择区
    RowLayout {
        anchors.centerIn: parent
        spacing: 40

        GameEntry {
            title: "🧛 吸血鬼幸存者"
            description: "旋转刀片 · 割草无双"
            entryColor: "#c0392b"
            onClicked: console.log("进入吸血鬼幸存者")
        }

        GameEntry {
            title: "🐱 合成猫meme"
            description: "物理掉落 · 合成西瓜"
            entryColor: "#8e44ad"
            onClicked: console.log("进入合成猫meme")
        }

        GameEntry{
            title:"🃏 斗地主"
            description: "欢乐斗地主·经典牌类"
            entryColor: "blue"
            onClicked: console.log("进入斗地主")
        }
    }

    component GameEntry: Rectangle {
        property string title: ""
        property string description: ""
        property color entryColor: "#888888"
        signal clicked

        width: 250
        height: 200
        radius: 16
        color: entryColor
        opacity: mouseArea.containsMouse ? 0.85 : 1.0
        scale: mouseArea.containsMouse ? 1.05 : 1.0

        Behavior on scale { NumberAnimation { duration: 150 } }

        MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: parent.clicked()
        }

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 8

            Text {
                text: title ? title : ""
                font.pixelSize: 22
                font.bold: true
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }

            Text {
                text: description ? description : ""
                font.pixelSize: 14
                color: "white"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}