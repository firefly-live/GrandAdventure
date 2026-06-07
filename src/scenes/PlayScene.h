//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:  游戏进行当中的场景
//create time : 2026-06-07 14:57:39


#pragma once
#include "../core/Scene.h"
#include <QVector>
#include <QRectF>

struct Obstacle {
    QRectF rect;
};

class PlayScene : public Scene {
    Q_OBJECT
public:
    explicit PlayScene(QObject* parent = nullptr);
    void onEnter() override;//进入场景后加载场景中障碍物的坐标,便于碰撞检测
    void update(int deltaMs) override;
    void draw() override {}



    // 供 MainWindow 调用来设置移动方向
    void setMoveDirection(const QPointF& dir);

private:
    void loadObstacles(const QString& path);
    void movePlayer(const QPointF& delta); //传入新坐标然后检测---防止玩家移动出地图
    bool collidesWithObstacles() const;

    QRectF m_mapBounds;//地图边界操作

    QVector<Obstacle> m_obstacles;  //场景中的碰撞障碍物
    QRectF m_playerRect;      // 玩家碰撞箱（位置+宽高）
    QPointF m_moveDir;        // 单位方向
    float m_speed = 5.0f;
};
