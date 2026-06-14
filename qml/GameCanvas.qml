import QtQuick 2.0

Rectangle {
    width: 1625
    height: 968

    // 背景图片（静态）
    Image {
        id: background
        source: "../Resource/bgc/background.png"
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }

    // 玩家图片
    Image {
        id: playerImage
        width: 80
        height: 80
        x: 0
        y: 0
        // Rectangle {
        //     anchors.fill: parent
        //     color: "transparent"
        //     border.color: "red"
        //     border.width: 2
        // }
    }

    // // 调试用半透明矩形（跟随移动，验证位置）
    // Rectangle {
    //     x: playerImage.x
    //     y: playerImage.y
    //     width: 80
    //     height: 80
    //     color: "red"
    //     opacity: 0.3
    // }

    // ---------- 位置更新定时器（每帧）----------
    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            playerImage.x = playScene.playerX()
            playerImage.y = playScene.playerY()
        }
    }

    // ---------- 动画帧定时器（10fps，即100ms）----------
    property int frameIndex: 0
    property int currentDir: 0
    property bool currentMoving: false

    Timer {
        interval: 80
        running: true
        repeat: true
        onTriggered: {
            // 更新方向和移动状态（用于选择正确的图片组）
            currentDir = playScene.getAnimDir()
            currentMoving = playScene.isMoving()

            // 帧索引无条件循环 0..3
            frameIndex = (frameIndex + 1) % 4

            updatePlayerImage()
        }
    }

    function updatePlayerImage() {
        var prefix = currentMoving ? "run" : "idle"
        var dirStr = ""
        // 根据动画方向枚举值 (Idle=0, Left=1, Right=2, Up=3, Down=4)
        switch (currentDir) {
            case 1: dirStr = "left"; break
            case 2: dirStr = "right"; break
            case 3: dirStr = "back"; break   // Up
            case 4: dirStr = "front"; break  // Down
            default: dirStr = "right"        // Idle 默认右方向
        }
        var fileName = "hajimi_" + prefix + "_" + dirStr + "_" + (frameIndex+1) + ".png"
        var fullPath = "../Resource/role/hajimi/" + fileName
        playerImage.source = fullPath
        // 可选调试输出
        // console.log("Anim:", fileName, "pos:", playerImage.x, playerImage.y)
    }
}
