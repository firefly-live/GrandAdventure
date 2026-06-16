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
#include "../gameobjects/Bullet.h"
#include<QVariantMap>

#include "../gameobjects/ExpOrb.h"
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

    Q_PROPERTY(int playerHp READ playerHp NOTIFY playerHpChanged)


    //--------------------------------------------子弹类----------------------------------
    Q_PROPERTY(QVariantList bullets READ bullets NOTIFY bulletsChanged)
    QVariantList bullets() const;

    Q_INVOKABLE void shootBullet(const QPointF& target);//射击敌人


    //--------------------------------------------------敌人类---------------------------

    Q_PROPERTY(QVariantList enemies READ enemies NOTIFY enemiesChanged);



    //-----------------------------------------------升级相关-----------------------------------------
    Q_PROPERTY(int level READ level NOTIFY statsChanged)
    Q_PROPERTY(int currentExp READ currentExp NOTIFY statsChanged)
    Q_PROPERTY(int expToNext READ expToNext NOTIFY statsChanged)
    Q_PROPERTY(float bulletDamage READ bulletDamage NOTIFY statsChanged)
    Q_PROPERTY(float speed READ speed NOTIFY statsChanged)
    Q_PROPERTY(float penetrationChance READ penetrationChance NOTIFY statsChanged)

    Q_PROPERTY(QVariantList expOrbs READ expOrbs NOTIFY expOrbsChanged)
    QVariantList expOrbs()  const;



    int level() const { return m_level; }
    int currentExp() const { return m_currentExp; }
    int expToNext() const { return m_expToNextLevel; }
    float bulletDamage() const { return m_bulletDamage; }
    float speed() const { return m_speed; }
    float penetrationChance() const { return m_penetrationChance; }
    Q_INVOKABLE void applyUpgrade(int index); // 由QML调用

    Q_PROPERTY(int maxHp READ maxHp NOTIFY statsChanged)
    int maxHp() const { return m_maxHp; }


    //----------------------------------------暂停相关
    void setUpgrading(bool upgrading);
    bool isUpgrading() const { return m_isUpgrading;}



public:
    explicit PlayScene(QObject* parent = nullptr);
    void onEnter() override;//进入场景后加载场景中障碍物的坐标,便于碰撞检测
    void update(int deltaMs) override;
    void draw() override {}
    int playerHp() const { return m_playerHp; }
    bool collidesWithObstacles() const;
    bool collidesWithObstacles(const QRectF& rect) const;//重载检测,用于测试敌人对障碍物碰撞


    // 供 MainWindow 调用来设置移动方向--角色移动相关
    void setMoveDirection(const QPointF& dir);
    QRectF playerRect() const { return m_playerRect; }//暴露给windwo,然后window对象player对象的坐标属性,方便传递给qml,修改qml数据
    void onKeyPress(Qt::Key key);
    void onKeyRelease(Qt::Key key);
    AnimDir animDirection() const { return m_animDir; }

    //--------------------------------------------------敌人类

    // QVariantList enemies() const;
    QRectF mapBounds() const { return m_mapBounds; }


signals:

    //-----------子弹类
     void bulletsChanged();

    void playerRectChanged();  // 必须在修改 m_playerRect 后发射
  void playerHpChanged();
    //--------------------敌人出现
   void enemiesChanged();

  //-----------------------------------------------升级相关-----------------------------------------
   void statsChanged();
   void upgradeRequested(const QStringList& options); // 通知QML弹出升级选择
    void expOrbsChanged();


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

    void handlePlayerCollision();
    void cleanupDeadObjects();



    //--------------------------------------------------敌人类--------------------------------------------



    QList<GameObject*> m_objects;
    QVariantList enemies() const;   // 实现从 m_objects 过滤敌人
    // 生成敌人时创建 PaimonEnemy 对象
    void spawnEnemy();
    void updateGameObjects(int deltaMs);
    void handleCollisions();


    //-------------------------------子弹射击类-----------------------------------------------------
    // 成员

    int m_playerHp = 1000;
    void handleCollisionsWithBullets(); // 处理子弹与敌人碰撞
    void handleBulletObstacleCollision(); //子弹和障碍物
//-----------------------------------------------升级相关-----------------------------------------

    void addExp(int value);
    void handleExpOrbCollection();
    QStringList generateUpgradeOptions();
    void upgradeLevel(); // 升级核心逻辑




    // 玩家属性
    int m_level = 1;
    int m_currentExp = 0;
    int m_expToNextLevel = 120; // 100 + level*20
    int m_maxHp = 1000;
    int m_bulletDamage = 100;
    float m_penetrationChance = 0.0f; // 0~1
     QStringList m_currentUpgradeOptions;   // 存储当前升级的三个选项
       bool m_isUpgrading = false;
};

