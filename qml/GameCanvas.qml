import QtQuick 2.0

Rectangle {
    width: 1625
    height: 968

    // 背景图片（静态）
    Image {
        id: background
        source: "../Resource/bgc/background.png"   // 实际路径
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }

    // 玩家图片（动态位置由 C++ 更新）
    Image {
        id: playerImage
        source: "../Resource/role/hajimi/hajimi_idle_right_1.png"
        width: 80
        height: 80
        x: playerX
        y: playerY
        z: 1
    }

    // 暴露给 C++ 修改的坐标属性----修改位置
    property real playerX: 100
    property real playerY: 100
}
