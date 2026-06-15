//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:  游戏进行当中的场景
//create time : 2026-06-07 14:57:39


#pragma once
#include "../core/Scene.h"
#include <QVector>
#include <QRectF>
#include "../core/GameObject.h"
#include "../gameobjects/Enemy.h"

#include "../gameobjects/PaimonEnemy.h"
//敌人的动态列表方向

// struct Enemy {
//     QRectF rect;
//     float speed;
//     int direction;      // 1左 2右
//     int frameIndex;     // 当前动画帧 (0~4)
// };





//增加角色移动方位
enum class AnimDir { Idle, Left, Right, Up, Down };







struct Obstacle {
    QRectF rect;
};

class PlayScene : public Scene {
    Q_OBJECT

public:
    Q_PROPERTY(QRectF playerRect READ playerRect NOTIFY playerRectChanged);//暴露玩家坐标给qml直接读取,防止注册导致渲染错误
    Q_INVOKABLE qreal playerX() const { return m_playerRect.x(); }
    Q_INVOKABLE qreal playerY() const { return m_playerRect.y(); }
    //标识移动还是禁止
    Q_INVOKABLE qreal  isMoving() const { return m_moveDir.x() != 0 || m_moveDir.y() != 0; }
    Q_INVOKABLE qreal  getAnimDir() const { return static_cast<int>(m_animDir); }


    //--------------------------------------------------敌人类

    Q_PROPERTY(QVariantList enemies READ enemies NOTIFY enemiesChanged);
   // Q_PROPERTY(QVariantList gameObjects READ gameObjects NOTIFY gameObjectsChanged);


public:
    explicit PlayScene(QObject* parent = nullptr);
    void onEnter() override;//进入场景后加载场景中障碍物的坐标,便于碰撞检测
    void update(int deltaMs) override;
    void draw() override {}



    // 供 MainWindow 调用来设置移动方向--角色移动相关
    void setMoveDirection(const QPointF& dir);
    QRectF playerRect() const { return m_playerRect; }//暴露给windwo,然后window对象player对象的坐标属性,方便传递给qml,修改qml数据
    void onKeyPress(Qt::Key key);
    void onKeyRelease(Qt::Key key);
    AnimDir animDirection() const { return m_animDir; }
    bool collidesWithObstacles() const;
    bool collidesWithObstacles(const QRectF& rect) const;//重载检测,用于测试敌人对障碍物碰撞


    //--------------------------------------------------敌人类

    // QVariantList enemies() const;
     QVariantList gameObjects() const;
    QRectF mapBounds() const { return m_mapBounds; }


signals:
    void playerRectChanged();  // 必须在修改 m_playerRect 后发射

    //--------------------敌人出现
   void enemiesChanged();
     //void gameObjectsChanged();

private:
    void loadObstacles(const QString& path);
    void movePlayer(const QPointF& delta); //传入新坐标然后检测---防止玩家移动出地图


    QRectF m_mapBounds;//地图边界操作

    QVector<Obstacle> m_obstacles;  //场景中的碰撞障碍物
    QRectF m_playerRect;      // 玩家碰撞箱（位置+宽高）
    QPointF m_moveDir;        // 单位方向
    float m_speed = 5.0f;


// 供 MainWindow 调用来设置移动方向--角色移动相关
    void updateMovement();  // 根据按键组合更新 m_moveDir 和 m_animDir
    bool m_left = false, m_right = false, m_up = false, m_down = false;
    AnimDir m_animDir = AnimDir::Idle;
    AnimDir m_lastMoveDir = AnimDir::Right;  // 记录最后一次移动的方向，默认右




    //--------------------------------------------------敌人类
    // void updateEnemies(int deltaMs);    //更新敌人--碰撞检测什么的
    // void spawnEnemy();              //生成敌人
    // void avoidEnemyCollision();     //碰撞箱子
    // // QList<Enemy> m_enemies;         //敌人列表
    // int m_enemySpawnCounter = 0;
    // float m_enemySpeed = 1.0f;      //敌人速度
    // mutable QVariantList m_enemiesCache;


    QList<GameObject*> m_objects;
    QVariantList enemies() const;   // 实现从 m_objects 过滤敌人
    // 生成敌人时创建 PaimonEnemy 对象
    void spawnEnemy();
    void updateGameObjects(int deltaMs);
    void handleCollisions();



};
