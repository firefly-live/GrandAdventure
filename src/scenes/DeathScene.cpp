#include "DeathScene.h"
#include "../core/SceneManager.h"
#include "../scenes/PlayScene.h"
#include <QRandomGenerator>
#include <QDebug>
#include<Qdir>
DeathScene::DeathScene(QObject* parent) : Scene(parent) {
    // 视频文件列表（请根据实际路径修改）
     m_videoList.clear();
}

void DeathScene::onEnter() {
    qDebug() << "DeathScene entered";
    QString path = getRandomVideoPath();
    // qDebug() << "Current working dir:" << QDir::currentPath();
    // qDebug() << "Video path:" << QDir::current().absoluteFilePath("../../Resource/videos/attack_kulou.MP4");
    qDebug()<<path;
    emit videoChanged(path);
}

void DeathScene::onExit() {
    qDebug() << "DeathScene exited";
}

QString DeathScene::getRandomVideoPath() const {
    if (m_videoList.isEmpty()) return "";
    int index = QRandomGenerator::global()->bounded(m_videoList.size());
    return m_videoList[index];
}

void DeathScene::restartGame() {
    // 获取 PlayScene 实例并重置
    PlayScene* play = dynamic_cast<PlayScene*>(SceneManager::instance()->findTypeScene(SceneType::Play));
    if (play) {
        play->resetGame();
    }
    // 切换到 PlayScene
    SceneManager::instance()->switchTo(SceneType::Play);
}

void DeathScene::quitToMenu() {
    SceneManager::instance()->switchTo(SceneType::Menu);
}
