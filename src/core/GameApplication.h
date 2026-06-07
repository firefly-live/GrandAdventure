//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models: 用于实现游戏得主循环,转化控制操作->每隔一段时间对输入事件进行转发,让场景管理器获取到更新时长是多久
//比ru让mainwindwo吧信息转发给场景管理器,设置间隔在该模块中
//create time : 2026-06-07 14:09:58

#pragma once
#include <QObject>
#include <QTimer>

class MainWindow;


class GameApplication : public QObject {
    Q_OBJECT
public:
    static GameApplication* instance();
    void start();          // 启动主循环
    void stop();

private:
    GameApplication() = default;
    QTimer m_timer;//计时器,每隔一段时间检测输入或者信号得传递操作
    void onTimeout();
};
