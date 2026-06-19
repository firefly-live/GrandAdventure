import QtQuick 2.0
import QtQuick.Controls
import QtQuick 2.0
import QtQuick.Controls
import QtMultimedia
Rectangle {
    width: 1625
    height: 968

    // ---- 背景 ----
    Image {
        id: background
        source: "../Resource/bgc/background.png"
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
    }

    // ---- 玩家 ----
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
            source: "../Resource/role/hajimi/hajimi_idle_right_1.png"
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
    }

    // ---- 玩家位置更新 ----
    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            playerContainer.x = playScene.playerX()
            playerContainer.y = playScene.playerY()
        }
    }

    // ---- 玩家动画属性 ----
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
            case 3: dirStr = "back"; break
            case 4: dirStr = "front"; break
            default: dirStr = "right"
        }
        var fileName = "hajimi_" + prefix + "_" + dirStr + "_" + (playerFrameIndex+1) + ".png"
        playerImage.source = "../Resource/role/hajimi/" + fileName
    }

    // ---- 血量条 ----
    Rectangle {
        width: 200; height: 30
        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
        z:10
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

    // ---- 瞄准镜 ----
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.BlankCursor

        Item {
            id: crosshair
            width: 20
            height: 20
            visible: true
            z: 100
            Image {
                width: 40
                height: 40
                source: "../Resource/role/hajimi/aim.png"
            }
        }

        onPositionChanged: {
            crosshair.x = mouseX - 10
            crosshair.y = mouseY - 10
        }
        onClicked: {
            soundManager.playShoot()
            playScene.shootBullet(Qt.point(mouseX, mouseY))
        }
    }

    // ---- 子弹 ----
    Repeater {
        model: playScene.bullets
        delegate: Image {
            width: 30
            height: 30
            source: "../Resource/weapen/sun/sun_ex.png"
            x: modelData.x - width/2
            y: modelData.y - height/2
        }
    }

    // ---- 技能按钮（E） ----
    Item {
        id: skillButton
        width: 80
        height: 80
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 30

        Rectangle {
            anchors.fill: parent
            radius: width/2
            color: "#333333"
            border.color: "white"
            border.width: 2
        }

        Image {
            source: "../Resource/role/hajimi/attack_e.png"
            anchors.fill: parent
            anchors.margins: 10
            fillMode: Image.PreserveAspectFit
            opacity: playScene.skillReady ? 1.0 : 0.5
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            Canvas {
                id: cooldownCanvas
                anchors.fill: parent
                visible: !playScene.skillReady
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    var centerX = width/2
                    var centerY = height/2
                    var radius = width/2
                    var progress = (playScene.skillCooldownRemaining > 0) ? (playScene.skillCooldownRemaining / playScene.skillCooldownMs) : 0.0
                    var startAngle = -Math.PI/2
                    var endAngle = startAngle + (1 - progress) * 2 * Math.PI
                    ctx.beginPath()
                    ctx.moveTo(centerX, centerY)
                    ctx.arc(centerX, centerY, radius, startAngle, endAngle)
                    ctx.closePath()
                    ctx.fillStyle = "#AA000000"
                    ctx.fill()
                }
            }
        }

        Text {
            anchors.centerIn: parent
            text: playScene.skillReady ? "E" : Math.ceil(playScene.skillCooldownRemaining / 1000) + "s"
            color: "white"
            font.bold: true
            font.pixelSize: 18
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (playScene.skillReady) {
                     soundManager.playESkill()   // 先播放音效
                    playScene.castSkill()
                }
            }
        }
    }

    // ---- 爆炸特效 ----
    ListModel {
        id: effectModel
    }

    function addExplosion(x, y) {
        effectModel.append({
            x: x,
            y: y,
            frame: 0,
            totalFrames: 4
        })
    }

    Repeater {
        model: effectModel
        delegate: Item {
            z:10
            Image {
                source: "../Resource/weapen/sun/sun_explode_" + (model.frame + 1) + ".png"
                width: 80
                height: 80
                x: model.x - 30
                y: model.y - 30
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
            soundManager.playHit()        // 击中音效
            //soundManager.playEnemyHit()   // 敌人惨叫
            addExplosion(x, y)
        }
    }

    // ---- Q技能按钮 ----
    Item {
        id: machineGunButton
        width: 80
        height: 80
        anchors.right: skillButton.left
        anchors.bottom: parent.bottom
        anchors.margins: 30
        anchors.rightMargin: 20

        Rectangle {
            anchors.fill: parent
            radius: width/2
            color: "#333333"
            border.color: "white"
            border.width: 2
        }

        Image {
            source: "../Resource/role/hajimi/attack_q.png"
            anchors.fill: parent
            anchors.margins: 10
            fillMode: Image.PreserveAspectFit
            opacity: playScene.machineGunReady ? 1.0 : 0.5
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            Canvas {
                id: eCooldownCanvas
                anchors.fill: parent
                visible: !playScene.machineGunReady
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    var centerX = width/2
                    var centerY = height/2
                    var radius = width/2
                    var progress = (playScene.machineGunCooldownRemaining > 0) ? (playScene.machineGunCooldownRemaining / playScene.machineGunCooldownMs) : 0.0
                    var startAngle = -Math.PI/2
                    var endAngle = startAngle + (1 - progress) * 2 * Math.PI
                    ctx.beginPath()
                    ctx.moveTo(centerX, centerY)
                    ctx.arc(centerX, centerY, radius, startAngle, endAngle)
                    ctx.closePath()
                    ctx.fillStyle = "#AA000000"
                    ctx.fill()
                }
            }
        }

        Text {
            anchors.centerIn: parent
            text: playScene.machineGunReady ? "q" : Math.ceil(playScene.machineGunCooldownRemaining / 1000) + "s"
            color: "white"
            font.bold: true
            font.pixelSize: 18
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (playScene.machineGunReady) {
                    soundManager.playQSkill()   // 先播放音效
                    playScene.castMachineGun()
                }
            }
        }
    }

    // ---- 敌人 ----
    Repeater {
        model: playScene.enemies
        delegate: Item {
            Image {
                source: "../Resource/role/paimeng/shadow.png"
                width: modelData.width * 0.6
                height: modelData.height * 0.15
                x: modelData.x + (modelData.width - width) / 2
                y: modelData.y + modelData.height - height + 10
                opacity:1
                z: 0
            }

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

            Image {
                source: {
                    var dirStr = (modelData.direction === 1) ? "left" : "right"
                    return "../Resource/role/paimeng/attack_" + dirStr + ".png"
                }
                width: modelData.width
                height: modelData.height
                x: modelData.x
                y: modelData.y
                opacity: modelData.isFlashing ? 1 : 0.0
                z: 2
            }

            Text {
                text: modelData.hp
                color: "red"
                font.pixelSize: 14
                font.bold: true
                anchors.bottom: paimong.top
                anchors.horizontalCenter: paimong.horizontalCenter
                visible: modelData.hp < 1000
                z: 2
            }
        }
    }

    // ---- 升级面板 ----
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
                btn1.text = options[0]
                btn2.text = options[1]
                btn3.text = options[2]
                upgradePanel.visible = true
            }
        }
    }

    // ---- 经验条 ----
    Rectangle {
        id: expBar
        x: 20; y: 20
        z:10
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

    // ---- 经验球 ----
    Repeater {
        model: playScene.expOrbs
        delegate: Item {
            Image {
                source: "../Resource/role/paimeng/shadow.png"
                width: 30; height: 20
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 10
                opacity: 1; z: 0
            }
            Image {
                source: "../Resource/role/paimeng/exp.png"
                width: 40; height: 40
                anchors.centerIn: parent
                z: 1
            }
            x: modelData.x - 20
            y: modelData.y - 20
        }
    }

    // ================= 死亡界面（覆盖层） =================
    Item {

            id: deathInterface
            width: 1625
            height: 968
            x: 0
            y: 0
            visible: playScene.gameOver
            z: 1000

            onVisibleChanged: {
                    if (visible) {
                        soundManager.toggleBgm(false)   // 暂停背景音乐
                                  mediaPlayer.play()              // 播放死亡视频
                    } else {
                        soundManager.toggleBgm(true)    // 恢复背景音乐
                                   mediaPlayer.stop()
                    }
                }


        // ---- 视频背景 ----
            VideoOutput {
                width: 1625
                height: 968
                id: videoOutput
                anchors.fill: parent
            }

            MediaPlayer {
                id: mediaPlayer
                videoOutput: videoOutput
                source: "../Resource/videos/attack_manbo.MP4"

                loops: MediaPlayer.Infinite
                audioOutput: AudioOutput {
                    volume: 1.0
                    muted: false
                }
            }

        // ---- 半透明遮罩 + 按钮（保持在视频上方） ----
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: 0.33
             anchors.topMargin: 580   // 顶部留出80像素

            Column {
                anchors.centerIn: parent
                spacing: 30
                Text {
                    text: "游戏结束"
                    color: "white"
                    font.pixelSize: 48
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Button {
                    text: "重新开始"
                    onClicked: {
                        playScene.resetGame()
                        playScene.setGameOver(false)
                    }
                    width: 200; height: 60
                    background: Rectangle { color: "lightgreen"; radius: 10 }
                    contentItem: Text { text: "重新开始"; anchors.centerIn: parent; color: "black"; font.pixelSize: 20 }
                }
                Button {
                    text: "退出"
                    onClicked: playScene.quitGame()
                    width: 200; height: 60
                    background: Rectangle { color: "lightcoral"; radius: 10 }
                    contentItem: Text { text: "退出"; anchors.centerIn: parent; color: "black"; font.pixelSize: 20 }
                }
            }
        }


    }

//    ================= 音效管理 =================




    Item {
        id: soundManager
        visible: false

        // 所有音效使用 SoundEffect（仅 WAV）
        SoundEffect {
            id: shootSound
            source: "../Resource/sounds/gun-shot.wav"
            volume: 0.3
            // 加载完成时立即播放并停止，强制解码
            Component.onCompleted: {
                play()
                stop()
            }
        }

        SoundEffect {
            id: hitSound
            source: "../Resource/sounds/exloded.wav"
            volume: 0.2
            Component.onCompleted: { play(); stop() }
        }

        SoundEffect {
            id: enemyHitSound
            source: "../Resource/sounds/paiMone_attacked.wav"
            volume: 0.5
            Component.onCompleted: { play(); stop() }
        }

        SoundEffect {
            id: playerHitSound
            source: "../Resource/sounds/player_attack.wav"
            volume: 0.7
            Component.onCompleted: { play(); stop() }
        }

        SoundEffect {
            id: qSkillSound
            source: "../Resource/sounds/q_skill.wav"
            volume: 0.35
            Component.onCompleted: { play(); stop() }  // 预加载
        }

        SoundEffect {
            id: eSkillSound
            source: "../Resource/sounds/e_skill.wav"
            volume: 1
            Component.onCompleted: { play(); stop() }  // 预加载
        }

        // 背景音乐仍用 MediaPlayer（可流式播放，无阻塞）
        MediaPlayer {
            id: bgmPlayer
            source: "../Resource/sounds/bgm.mp3"
            audioOutput: AudioOutput { volume: 0.5 }
            loops: MediaPlayer.Infinite
            autoPlay: true
        }



        function playESkill() { eSkillSound.play() }
        function playQSkill() { qSkillSound.play() }
        function playShoot() { shootSound.play() }
        function playHit() { hitSound.play() }
        function playEnemyHit() { enemyHitSound.play() }
        function playPlayerHit() { playerHitSound.play() }
        function toggleBgm(play) {
            if (play) bgmPlayer.play()
            else bgmPlayer.pause()
        }
    }


    //q节能触发
    Connections {
        target: playScene
        function onMachineGunCast() {
            soundManager.playQSkill()
        }
    }

    Connections {
        target: playScene
        function onPlayerHurt() {
            soundManager.playPlayerHit()
        }
    }

    Connections {
        target: playScene
        function onSkillCast() {
            soundManager.playESkill()
        }
    }






} // end Rectangle
