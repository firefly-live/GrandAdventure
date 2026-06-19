#include "SceneManager.h"
#include "Scene.h"
#include "../ui/MainWindow.h"
SceneManager* SceneManager::instance() {
    static SceneManager mgr;
    return &mgr;
}

SceneManager::~SceneManager() {
    for (auto& pair : m_scenes) {
        delete pair.second;
    }
}

void SceneManager::registerScene(SceneType type, Scene* scene) {
    m_scenes[type] = scene;

}

void SceneManager::switchTo(SceneType type) {
    auto it = m_scenes.find(type);
    if (it == m_scenes.end()) return;//如果没找到键对应得值退出

    if (m_currentScene) m_currentScene->onExit();
    m_currentScene = it->second;
    if (m_currentScene) m_currentScene->onEnter();

    // //判断下然后切换
    // if (MainWindow::instance()) {
    //     MainWindow::instance()->loadSceneQML(type);
    // }
}

Scene* SceneManager::currentScene() const {
    return m_currentScene;
}

void SceneManager::update(int deltaMs) {
    if (m_currentScene) m_currentScene->update(deltaMs);//用于更新输入数据
}

void SceneManager::draw() {
    if (m_currentScene) m_currentScene->draw();//更新数据后就绘制
}


Scene* SceneManager::findTypeScene(SceneType type) {
    auto it = m_scenes.find(type);
    if (it != m_scenes.end()) {
        qDebug() << "findTypeScene found type:" << static_cast<int>(type) << "scene:" << it->second;
        return it->second;
    }
    qDebug() << "findTypeScene NOT found type:" << static_cast<int>(type);
    return nullptr;
}
