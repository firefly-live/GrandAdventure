import QtQuick 2.0

Rectangle {
    id: root
    width: 1625
    height: 968

    // 强制设置固定大小（关键！）
    implicitWidth: 1625
    implicitHeight: 968

    // 背景图片
    Image {
        id: background
        source: "../Resource/bgc/background.png"
        width: parent.width
        height: parent.height
        fillMode: Image.Stretch
    }

    // 玩家图片
    Image {
        id: playerImage
        source: "../Resource/role/hajimi/hajimi_idle_right_1.png"

        x: playScene ? playScene.playerRect.x : 0
        y: playScene ? playScene.playerRect.y : 0
        width: playScene ? playScene.playerRect.width : 80
        height: playScene ? playScene.playerRect.height : 80

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "red"
            border.width: 2
        }
    }

    // 调试文字
    Text {
        x: 10
        y: 10
        color: "white"
        font.pixelSize: 16
        text: "位置: (" + playerImage.x.toFixed(0) + ", " + playerImage.y.toFixed(0) + ")"
        z: 10
    }
}
