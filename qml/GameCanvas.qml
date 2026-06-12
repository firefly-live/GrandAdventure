import QtQuick 2.0

Rectangle {
    width: 1625
    height: 968


    // QML Timer 负责每帧更新坐标
    // Timer {
    //     interval: 16
    //     repeat: true
    //     running: true
    //     onTriggered: {
    //         // 直接从 C++ 对象拉取最新坐标
    //         playerImage.x +=1
    //         playerImage.y +=1
    //     }
    // }



    // 背景图片（静态）
    Image {
        id: background
        source: "../Resource/bgc/background.png"   // 实际路径
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }

    // 玩家图片
        Image {
            id: playerImage
            source: "../Resource/role/hajimi/hajimi_idle_right_1.png"

            // 直接绑定 playScene 的属性
            x: playScene ? playScene.playerRect.x : 0
            y: playScene ? playScene.playerRect.y : 0
            width: playScene ? playScene.playerRect.width : 80
            height: playScene ? playScene.playerRect.height : 80

            // 调试边框（可保留）
            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border.color: "red"
                border.width: 2
            }
        }

    Rectangle {
        x: playerImage.x   // 或直接绑定 playerX
        y: playerImage.y
        width: 80
        height: 80
        color: "red"
        opacity: 0.8
    }


    // // 暴露给 C++ 修改的坐标属性----修改位置
    // property real playerX: 100
    // property real playerY: 100
    // onPlayerXChanged: console.log("QML: playerImage.x changed to   ",playerImage.x)
}
