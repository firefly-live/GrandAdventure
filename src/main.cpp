#include <QApplication>
#include "ui/MainWindow.h"
#include "core/GameApplication.h"
#include "core/SceneManager.h"
#include "scenes/CharacterSelectScene.h"
#include"scenes/PlayScene.h";
#include<QQuickStyle>
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 设置样式为 Basic, Fusion, Material 等
    QQuickStyle::setStyle("Fusion");   // 或者 "Basic"、"Material"


    // 注册场景---一次性注册玩,防止注册又删除导致性能损耗太大
    SceneManager::instance()->registerScene(SceneType::CharacterSelect, new CharacterSelectScene());
    SceneManager::instance()->registerScene(SceneType::Play, new PlayScene());


 app.setOverrideCursor(QCursor(Qt::BlankCursor));

    //切换到游戏场景测试
     SceneManager::instance()->switchTo(SceneType::Play);

    // 启动主循环
    GameApplication::instance()->start();

    MainWindow w;
    w.show();

    return app.exec();
}
