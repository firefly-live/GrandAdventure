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

    // ================= 玩家 =================
    Image {
        id: playerImage
        width: 80
        height: 80
        x: 0
        y: 0
    }

    // 玩家位置更新
    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            playerImage.x = playScene.playerX()
            playerImage.y = playScene.playerY()
        }
    }

    property int playerFrameIndex: 0
    property int playerAnimDir: 0
    property bool playerMoving: false

    Timer {
        interval: 80
        running: true
        repeat: true
        onTriggered: {
            playerAnimDir = playScene.getAnimDir()
            playerMoving = playScene.isMoving()
            playerFrameIndex = (playerFrameIndex + 1) % 4
            updatePlayerImage()

        }
    }

    function updatePlayerImage() {
        var prefix = playerMoving ? "run" : "idle"
        var dirStr = ""
        switch (playerAnimDir) {
            case 1: dirStr = "left"; break
            case 2: dirStr = "right"; break
            case 3: dirStr = "back"; break   // Up
            case 4: dirStr = "front"; break  // Down
            default: dirStr = "right"
        }
        var fileName = "hajimi_" + prefix + "_" + dirStr + "_" + (playerFrameIndex+1) + ".png"
        playerImage.source = "../Resource/role/hajimi/" + fileName
    }

    // ================= 敌人 =================
    Repeater {
        model: playScene.enemies
        delegate: Item {
            Image {
                width: modelData.width
                height: modelData.height
                x: modelData.x
                y: modelData.y
                source: {
                    var dirStr = (modelData.direction === 1) ? "left" : "right"
                    var frame = modelData.frameIndex
                    return "../Resource/role/paimeng/paimon_" + dirStr + "_" + frame + ".png"
                }
                // Rectangle {
                //     anchors.fill: parent
                //     color: "transparent"
                //     border.color: "yellow"
                //     border.width: 1
                // }
            }
        }
    }
}
