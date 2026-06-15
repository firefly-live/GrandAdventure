//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:   游戏的主框架,只作为分发输入事件的操作
//create time : 2026-06-07 14:24:01


#pragma once
#include <QMainWindow>
#include <QQuickWidget>
#include<QKeyEvent>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    //标识角色场景是移动还是禁止
    Q_INVOKABLE int getAnimDir() const;
    Q_INVOKABLE bool isMoving() const;

public:
    MainWindow(QWidget* parent = nullptr);
    static MainWindow *instance();
    void updatePlayerPosition(const QPointF& pos);



    //按键操作
    void keyPressEvent(QKeyEvent* event);//不同场景下对于按键触发有不同实现逻辑
    void keyReleaseEvent(QKeyEvent* event);//不同场景下对于按键触发有不同实现逻辑

    //转发鼠标操作
    void mousePressEvent(QMouseEvent* event);


private:
    QQuickWidget* m_qmlWidget = nullptr;//加载aml显示对象
    QObject* m_qmlRoot = nullptr;//加载游戏对象信息
};
