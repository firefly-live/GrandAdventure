#pragma once
#include "../core/Scene.h"
#include <QObject>
#include <QStringList>

class DeathScene : public Scene {
    Q_OBJECT
public:
    explicit DeathScene(QObject* parent = nullptr);
    void onEnter() override;
    void onExit() override;
    void update(int deltaMs) override {}
    void draw() override {}

    Q_INVOKABLE void restartGame();   // 重新开始
    Q_INVOKABLE void quitToMenu();    // 退出到主菜单
    Q_INVOKABLE QString getRandomVideoPath() const; // 随机视频路径

signals:
    void videoChanged(QString path);  // 通知 QML 切换视频

private:
    QStringList m_videoList;
};
