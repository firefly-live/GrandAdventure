#include "GameApplication.h"
#include "SceneManager.h"
#include <QElapsedTimer>

GameApplication* GameApplication::instance() {
    static GameApplication app;//初始化管理游戏对象操作
    return &app;
}

void GameApplication::start() {
    QObject::connect(&m_timer, &QTimer::timeout, [this]() {
    SceneManager::instance()->update(16);//通过调用场景管理器当中的静态对象进行数据逻辑的更新
        // 触发渲染（后续可通过信号通知 QML）--->当数据处理完毕的时候,再让场景管理器对各自场景当中的信息进行更新渲染的操作,以便于达到动画的效果
    });
    m_timer.start(16); // 60 FPS
}

void GameApplication::stop() {
    m_timer.stop();
}
