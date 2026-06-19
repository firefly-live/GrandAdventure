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

#include<QTimer>
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

    // ======================== 构造与析构 ========================
    explicit PlayScene(QObject* parent = nullptr);

    Q_INVOKABLE void resetGame();   // 重置游戏状态

    // ======================== 玩家属性/状态 ========================
    //闪烁查看
    Q_PROPERTY(bool invincible READ isInvincible NOTIFY invincibleChanged);
    bool isInvincible() const { return m_invincible; }


    Q_PROPERTY(QRectF playerRect READ playerRect NOTIFY playerRectChanged);//暴露玩家坐标给qml直接读取,防止注册导致渲染错误
    Q_INVOKABLE qreal playerX() const { return m_playerRect.x(); }
    Q_INVOKABLE qreal playerY() const { return m_playerRect.y(); }
    //标识移动还是禁止
    Q_INVOKABLE qreal  isMoving() const { return m_moveDir.x() != 0 || m_moveDir.y() != 0; }
    Q_INVOKABLE qreal  getAnimDir() const { return static_cast<int>(m_animDir); }

    Q_PROPERTY(int playerHp READ playerHp NOTIFY playerHpChanged)
    int playerHp() const { return m_playerHp; }
    Q_PROPERTY(int maxHp READ maxHp NOTIFY statsChanged)
    int maxHp() const { return m_maxHp; }

    QRectF playerRect() const { return m_playerRect; }//暴露给windwo,然后window对象player对象的坐标属性,方便传递给qml,修改qml数据
    AnimDir animDirection() const { return m_animDir; }

    // ======================== 玩家移动/输入 ========================
    void setMoveDirection(const QPointF& dir);
    void onKeyPress(Qt::Key key);
    void onKeyRelease(Qt::Key key);

    // ======================== 子弹 ========================
    //--------------------------------------------子弹类----------------------------------
    Q_PROPERTY(QVariantList bullets READ bullets NOTIFY bulletsChanged)
    QVariantList bullets() const;

    Q_INVOKABLE void shootBullet(const QPointF& target);//射击敌人

    // ======================== 敌人 ========================
    //--------------------------------------------------敌人类---------------------------
    Q_PROPERTY(QVariantList enemies READ enemies NOTIFY enemiesChanged);
    QRectF mapBounds() const { return m_mapBounds; }

    // ======================== 升级与经验 ========================
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

    // ======================== 技能 ========================
    //技能散射
    Q_INVOKABLE void castSkill();  // 由QML或主窗口调用

    Q_PROPERTY(bool skillReady READ isSkillReady NOTIFY skillReadyChanged)
    Q_PROPERTY(int skillCooldownRemaining READ skillCooldownRemaining NOTIFY skillCooldownUpdated)

    bool isSkillReady() const { return m_skillReady; }
    int skillCooldownRemaining() const { return m_skillCooldownRemaining; }


    // E技能相关
    Q_PROPERTY(bool machineGunReady READ isMachineGunReady NOTIFY machineGunReadyChanged)
    bool isMachineGunReady() const { return m_machineGunReady; }

    Q_INVOKABLE void castMachineGun();   // 触发技能

    void fireOneBulletTowardsDirection();

    Q_PROPERTY(int machineGunCooldownRemaining READ machineGunCooldownRemaining NOTIFY machineGunCooldownUpdated)
    Q_PROPERTY(int machineGunCooldown READ machineGunCooldown CONSTANT)

    int machineGunCooldownRemaining() const;
    int machineGunCooldown() const { return m_machineGunCooldown; }

    // ======================== 暂停/升级状态 ========================
    //----------------------------------------暂停相关
    void setUpgrading(bool upgrading);
    bool isUpgrading() const { return m_isUpgrading;}

    // ======================== 场景通用接口 ========================
    void onEnter() override;//进入场景后加载场景中障碍物的坐标,便于碰撞检测
    void update(int deltaMs) override;
    void draw() override {}
    bool collidesWithObstacles() const;
    bool collidesWithObstacles(const QRectF& rect) const;//重载检测,用于测试敌人对障碍物碰撞


    Q_PROPERTY(bool gameOver READ isGameOver NOTIFY gameOverChanged)
    bool isGameOver() const { return m_gameOver; }
   Q_INVOKABLE void setGameOver(bool over);

    Q_INVOKABLE void quitGame();


signals:

    // ======================== 信号 ========================
    void invincibleChanged(bool);

    void playerRectChanged();  // 必须在修改 m_playerRect 后发射
    void playerHpChanged();

      void playerHurt();

    //-----------子弹类
    void bulletsChanged();

    void explosionAt(qreal x, qreal y);   // 爆炸特效信号

    //--------------------敌人出现
    void enemiesChanged();

    //-----------------------------------------------升级相关-----------------------------------------
    void statsChanged();
    void upgradeRequested(const QStringList& options); // 通知QML弹出升级选择
    void expOrbsChanged();

    //------------------技能散射
    void skillReadyChanged();
    void skillCooldownUpdated();

    void machineGunReadyChanged();

    void machineGunCooldownUpdated();

    void machineGunCast();   // Q技能触发信号

    void skillCast();   // E技能释放信号


      void gameOverChanged();

private:

        bool m_gameOver = false;
    // ======================== 内部方法 ========================
    void loadObstacles(const QString& path);
    void movePlayer(const QPointF& delta); //传入新坐标然后检测---防止玩家移动出地图

    // 供 MainWindow 调用来设置移动方向--角色移动相关
    void updateMovement();  // 根据按键组合更新 m_moveDir 和 m_animDir

    void handlePlayerCollision();
    void cleanupDeadObjects();

    //--------------------------------------------------敌人类--------------------------------------------
    void spawnEnemy();
    void updateGameObjects(int deltaMs);
    void handleCollisions();

    //-------------------------------子弹射击类-----------------------------------------------------
    void handleCollisionsWithBullets(); // 处理子弹与敌人碰撞
    void handleBulletObstacleCollision(); //子弹和障碍物

    //-----------------------------------------------升级相关-----------------------------------------
    void addExp(int value);
    void handleExpOrbCollection();
    QStringList generateUpgradeOptions();
    void upgradeLevel(); // 升级核心逻辑

    void startInvincible();

    // ======================== 成员变量 ========================
    QRectF m_mapBounds;//地图边界操作
    QVector<Obstacle> m_obstacles;  //场景中的碰撞障碍物
    QRectF m_playerRect;      // 玩家碰撞箱（位置+宽高）
    QPointF m_moveDir;        // 单位方向
    float m_speed = 5.0f;

    bool m_invincible = false;
    QTimer* m_invincibleTimer;

    bool m_left = false, m_right = false, m_up = false, m_down = false;
    AnimDir m_animDir = AnimDir::Idle;
    AnimDir m_lastMoveDir = AnimDir::Right;  // 记录最后一次移动的方向，默认右

    //--------------------------------------------------敌人类--------------------------------------------
    QList<GameObject*> m_objects;
    QVariantList enemies() const;   // 实现从 m_objects 过滤敌人




    //-----------------------------------------------升级相关-----------------------------------------
    // 玩家属性
    int m_playerHp = 1000;
    int m_level = 1;
    int m_currentExp = 0;
    int m_expToNextLevel = 120; // 100 + level*20
    int m_maxHp = 1000;
    int m_bulletDamage = 100;
    float m_penetrationChance = 0.0f; // 0~1
    QStringList m_currentUpgradeOptions;   // 存储当前升级的三个选项
    bool m_isUpgrading = false;

    //散射
    float m_skillAngleStep = 10.0f;  // 角度步长（度），默认为10辐射散开技能
    bool m_skillReady = true;
    int m_skillCooldownMs = 3000;          // 冷却时间（毫秒）
    int m_skillCooldownRemaining = 0;      // 剩余冷却时间（毫秒）
    QTimer* m_skillCooldownTimer;




    // E技能
    bool m_machineGunReady = true;
    bool m_machineGunActive = false;
    int m_machineGunDuration = 2000;          // 持续时间2秒
    int m_machineGunCooldown = 3000;          // 冷却3秒
    int m_machineGunFireInterval = 50;        // 每50ms发射一发
    QTimer* m_machineGunTimer;
    QTimer* m_machineGunCooldownTimer;
    QTimer* m_machineGunFireTimer;
    QTimer* m_machineGunCooldownUpdateTimer;

    float m_machineGunSpreadAngle = 70.0f;          // 总覆盖角度（度）
    int m_machineGunBulletsPerBurst = 7;            // 每波子弹数量



};
