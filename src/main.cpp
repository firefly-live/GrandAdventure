#include <QApplication>
#include "ui/MainWindow.h"
#include "core/GameApplication.h"
#include "core/SceneManager.h"
#include "scenes/CharacterSelectScene.h"
#include"scenes/PlayScene.h"
#include"scenes/DeathScene.h"
#include<QQuickStyle>
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QQuickStyle::setStyle("Fusion");

    // 先注册所有场景
    SceneManager::instance()->registerScene(SceneType::CharacterSelect, new CharacterSelectScene());
    SceneManager::instance()->registerScene(SceneType::Play, new PlayScene());
    SceneManager::instance()->registerScene(SceneType::Death, new DeathScene());



    // 然后创建主窗口（构造函数中会使用已注册的场景）

    MainWindow w;



      w.m_qmlWidget->setSource(QUrl("../../qml/GameCanvas.qml"));

      w.show();
    // 切换到游戏场景（构造函数默认已显示游戏，但可再次确认）
    SceneManager::instance()->switchTo(SceneType::Play);
    GameApplication::instance()->start();

    return app.exec();
}
