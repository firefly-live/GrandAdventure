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
    Item {
        id: playerContainer
        width: 80
        height: 80
        x: 0
        y: 0

        Image {
            id: playerImage
            width: 80
            height: 80
            anchors.fill: parent
            source: "../Resource/role/hajimi/hajimi_idle_right_1.png" // 初始图片
        }

        Timer {
            id: blinkTimer
            interval: 150
            running: playScene.invincible
            repeat: true
            onTriggered: {
                playerContainer.opacity = (playerContainer.opacity === 1.0) ? 0.2 : 1.0
            }
            onRunningChanged: {
                if (!running) playerContainer.opacity = 1.0
            }

        }
        // Text { text: "无敌: " + playScene.invincible; color: "white"; x: 10; y: 50 }
    }




    // 玩家位置更新
    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            playerContainer.x = playScene.playerX(); playerContainer.y = playScene.playerY();
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


    //子弹的repeater
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


    //子弹爆炸动画显示
    // 特效模型（存放所有爆炸特效）
    ListModel {
        id: effectModel
    }

    // 添加爆炸特效的函数（由 C++ 调用）
    function addExplosion(x, y) {
        effectModel.append({
            x: x,
            y: y,
            frame: 0,
            totalFrames: 4
        });
    }

    Repeater {
        model: effectModel
        delegate: Item {
             z:10
            Image {
                source: "../Resource/weapen/sun/sun_explode_" + (model.frame + 1) + ".png"
                width: 80
                height: 80
                x: model.x  -30 // 64/2
                y: model.y   -30

            }
            Timer {
                interval: 80
                running: true
                repeat: true
                onTriggered: {
                    var newFrame = model.frame + 1
                    if (newFrame >= model.totalFrames) {
                        effectModel.remove(index)
                    } else {
                        effectModel.setProperty(index, "frame", newFrame)
                    }
                }
            }
        }
    }

    Connections {
        target: playScene
        function onExplosionAt(x, y) {
            addExplosion(x, y);
            //console.log("attack bottom enemy"+" "+ x +" "+y);
        }
    }







    // ================= 敌人 =================
    Repeater {
        model: playScene.enemies
        delegate: Item {
            // 阴影（与敌人图片同级）
            Image {
                source: "../Resource/role/paimeng/shadow.png"
                width: modelData.width * 0.6          // 宽度为敌人的 60%
                height: modelData.height * 0.15       // 高度为敌人的 15%（扁平）
                x: modelData.x + (modelData.width - width) / 2   // 水平居中
                y: modelData.y + modelData.height - height+10       // 位于角色脚下
                opacity:1
                z: 0
            }

            // 敌人图片
            Image {
                id: paimong
                width: modelData.width
                height: modelData.height
                x: modelData.x
                y: modelData.y
                source: {
                    var dirStr = (modelData.direction === 1) ? "left" : "right"
                    var frame = modelData.frameIndex
                    return "../Resource/role/paimeng/paimon_" + dirStr + "_" + frame + ".png"
                }
                opacity: modelData.isDying ? (Math.floor(Date.now() / 100) % 2 === 0 ? 0.3 : 1.0) : 1.0
                z: 1
            }

            // 血量文字（保持在最上层）
            Text {
                text: modelData.hp
                color: "red"
                font.pixelSize: 14
                font.bold: true
                anchors.bottom: paimong.top   // 直接锚定到敌人图片
                anchors.horizontalCenter: paimong.horizontalCenter
                visible: modelData.hp < 1000
                z: 2
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

    //经验条显示

    Rectangle {
        id: expBar
        x: 20; y: 20
        width: 200; height: 20
        color: "gray"

        Rectangle {
            width: (playScene.level >= 20) ? parent.width : (playScene.currentExp * 1.0 / playScene.expToNext) * parent.width
            height: parent.height
            color: "blue"
        }

        Text {
            text: (playScene.level >= 20) ? "MAX" : "Lv." + playScene.level + " " + playScene.currentExp + "/" + playScene.expToNext
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 12
        }
    }


    //----------------经验求
    Repeater {
        model: playScene.expOrbs
        delegate: Item {
            // 阴影
            Image {
                source: "../Resource/role/paimeng/shadow.png"
                width: 30; height: 20
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 10   // 偏移
                opacity: 1; z: 0
            }
            // 经验球本体
            Image {
                source: "../Resource/role/paimeng/exp.png"
                width: 40; height: 40
                anchors.centerIn: parent
                z: 1
            }
            // 将 Item 的位置绑定到模型数据
            x: modelData.x - 20   // 因为本体的 width/2 = 20
            y: modelData.y - 20
        }
    }



}
