import QtQuick 2.0
import QtQuick.Controls
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


    //血量
    Rectangle {
        width: 200; height: 30
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        color: "red"
        border.color: "black"
        Rectangle {
            width: (playScene.playerHp * 1.0 / playScene.maxHp) * parent.width
            height: parent.height
            color: "green"
        }
        Text {
            anchors.centerIn: parent
            text: playScene.playerHp + " / " + playScene.maxHp
            color: "white"
            font.bold: true
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


    //射击的指针

    MouseArea {
        id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.BlankCursor   // 隐藏系统光标

        Item {
            id: crosshair
            width: 20
            height: 20
            visible: true
            z: 100
            // 移除 x/y 绑定，改为在 onPositionChanged 中设置
            Rectangle {
                width: 2; height: 20; color: "#80FF0000"; anchors.centerIn: parent; rotation: 45
            }
            Rectangle {
                width: 2; height: 20; color: "#80FF0000"; anchors.centerIn: parent; rotation: -45
            }
        }

        onPositionChanged: {
            crosshair.x = mouseX - 10
            crosshair.y = mouseY - 10
        }
        onClicked: {
            playScene.shootBullet(Qt.point(mouseX, mouseY))
        }
    }


    Repeater {
        model: playScene.bullets
        delegate: Image {
            width: 30    // 根据图片尺寸调整
            height: 30
            source: "../Resource/weapen/sun/sun_ex.png"
            x: modelData.x - width/2   // 使图片中心对齐子弹坐标
            y: modelData.y - height/2
        }
    }




    // ================= 敌人 =================
    Repeater {
        model: playScene.enemies   // 改为使用 gameObjects
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
                // // 调试边框
                //             Rectangle {
                //                 anchors.fill: parent
                //                 color: "transparent"
                //                 border.color: "yellow"
                //                 border.width: 2
                //             }
                Text {
                            text: modelData.hp
                            color: "red"
                            font.pixelSize: 14
                            font.bold: true
                            anchors.bottom: parent.top
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible: modelData.hp < 1000  // 只要不是满血就显示，即被击中后
                        }
            }
        }
    }




    //==============================================升级相关
    Rectangle {
        id: upgradePanel
        anchors.fill: parent
        color: "#AA000000"
        visible: false
        z: 200
        Column {
            anchors.centerIn: parent
            spacing: 20
            Text { text: "升级！选择一项能力"; color: "white"; font.pixelSize: 24 }
            Row {
                spacing: 20
                Button {
                    id: btn1
                    width: 180; height: 40
                    background: Rectangle { color: "lightblue"; radius: 5 }
                    contentItem: Text { text: btn1.text; anchors.centerIn: parent }
                    onClicked: { playScene.applyUpgrade(0); upgradePanel.visible = false; }
                }
                Button {
                    id: btn2
                    width: 180; height: 40
                    background: Rectangle { color: "lightblue"; radius: 5 }
                    contentItem: Text { text: btn2.text; anchors.centerIn: parent }
                    onClicked: { playScene.applyUpgrade(1); upgradePanel.visible = false; }
                }
                Button {
                    id: btn3
                    width: 180; height: 40
                    background: Rectangle { color: "lightblue"; radius: 5 }
                    contentItem: Text { text: btn3.text; anchors.centerIn: parent }
                    onClicked: { playScene.applyUpgrade(2); upgradePanel.visible = false; }
                }
            }
        }
        Connections {
            target: playScene
            function onUpgradeRequested(options) {
                btn1.text = options[0];
                btn2.text = options[1];
                btn3.text = options[2];
                upgradePanel.visible = true;
            }
        }
    }

    Rectangle {
        x: 20; y: 20
        width: 200; height: 20
        color: "gray"
        Rectangle {
            width: (playScene.currentExp * 1.0 / playScene.expToNext) * parent.width
            height: parent.height
            color: "blue"
        }
        Text {
            text: "Lv." + playScene.level + " " + playScene.currentExp + "/" + playScene.expToNext
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 12
        }
    }


    //----------------经验求
    Repeater {
        model: playScene.expOrbs
        delegate: Rectangle {
            width: 10; height: 10; color: "gold"
            x: modelData.x; y: modelData.y
        }
    }



}
