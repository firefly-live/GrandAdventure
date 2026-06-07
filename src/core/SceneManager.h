//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:   用作管理场景跳转得操作,交互逻辑,根据状态机来实现如何跳转以及怎么跳转
//一次性创建好场景,方便后续进行切换,不损耗性能和体验
//create time : 2026-06-07 11:41:06


#pragma once
#include <QObject>
#include <memory>
#include <unordered_map>

class Scene;

enum class SceneType {
    Menu,
    CharacterSelect,
    Play,
    Pause,
    Upgrade,
    Death
};//状态机所示得场景信息,用于切换

class SceneManager : public QObject {
    Q_OBJECT
public:
    static SceneManager* instance();
    ~SceneManager();
    // 注册场景（初始化时一次性调用）
    void registerScene(SceneType type, Scene* scene);

    // 切换场景
    void switchTo(SceneType type);

    // 获取当前场景
    Scene* currentScene() const;

    // 转发调用
    void update(int deltaMs);
    void draw();

private:
    SceneManager() = default;
     std::unordered_map<SceneType, Scene*> m_scenes;
    Scene* m_currentScene = nullptr;
};
