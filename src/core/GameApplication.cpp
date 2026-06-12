#include "GameApplication.h"
#include "SceneManager.h"
#include"../ui/MainWindow.h"
#include"../scenes/PlayScene.h"
#include <QElapsedTimer>

GameApplication* GameApplication::instance() {
    static GameApplication app;//初始化管理游戏对象操作
    return &app;
}

void GameApplication::start() {
    QObject::connect(&m_timer, &QTimer::timeout, [this]() {
        // 1. 更新游戏逻辑
        SceneManager::instance()->update(16);
        // 2. 更新 QML 坐标
        // if (auto scene = dynamic_cast<PlayScene*>(SceneManager::instance()->currentScene())) {
        //     if (MainWindow* w = MainWindow::instance()) {
        //         w->updatePlayerPosition(scene->playerRect().topLeft());
        //          // qDebug()<<"yes charcatr scene install";
        //     }
        // }

    });
    m_timer.start(16);
}

void GameApplication::stop() {
    m_timer.stop();
}
