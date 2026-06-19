#include "PlayScene.h"
#include"../core/SceneManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include<QLineF>
#include <QRandomGenerator>
#include <QPointF>
#include <cmath>
#include"../ui/MainWindow.h"

PlayScene::PlayScene(QObject* parent) : Scene(parent) {


    m_invincibleTimer = new QTimer(this);
    m_invincibleTimer->setSingleShot(true);
    connect(m_invincibleTimer, &QTimer::timeout, [this]() {
        m_invincible = false;
        emit invincibleChanged(false);   // 需要添加信号
    });

    //技能散射
    m_skillCooldownTimer = new QTimer(this);
    m_skillCooldownTimer->setInterval(100);  // 每100ms更新一次UI
    connect(m_skillCooldownTimer, &QTimer::timeout, [this]() {
        m_skillCooldownRemaining -= 100;
        if (m_skillCooldownRemaining <= 0) {
            m_skillCooldownRemaining = 0;
            m_skillReady = true;
            m_skillCooldownTimer->stop();
            emit skillReadyChanged();
        }
        emit skillCooldownUpdated();
    });


    // E技能定时器：控制持续时间
    // 机枪发射定时器
    m_machineGunFireTimer = new QTimer(this);
    m_machineGunFireTimer->setInterval(m_machineGunFireInterval);
    connect(m_machineGunFireTimer, &QTimer::timeout, [this]() {
        fireOneBulletTowardsDirection();   // 每50ms发射一颗
    });

    // 持续计时器
    m_machineGunTimer = new QTimer(this);
    m_machineGunTimer->setSingleShot(true);
    connect(m_machineGunTimer, &QTimer::timeout, [this]() {
        m_machineGunActive = false;
        m_machineGunFireTimer->stop();
        emit machineGunReadyChanged();
    });

    // 冷却计时器
    m_machineGunCooldownTimer = new QTimer(this);
    m_machineGunCooldownTimer->setSingleShot(true);
    connect(m_machineGunCooldownTimer, &QTimer::timeout, [this]() {
        m_machineGunReady = true;
        emit machineGunReadyChanged();
        emit machineGunCooldownUpdated(); // 最后更新一次
    });

    // 冷却更新定时器（每100ms通知QML更新剩余时间）
    m_machineGunCooldownUpdateTimer = new QTimer(this);
    m_machineGunCooldownUpdateTimer->setInterval(100);
    connect(m_machineGunCooldownUpdateTimer, &QTimer::timeout, [this]() {
        emit machineGunCooldownUpdated();
    });




    m_playerRect = QRectF(100, 100, 80, 80);
}

void PlayScene::onEnter() {
    qDebug() << "PlayScene entered";
    loadObstacles("../../Resource/bgc/obstacles.json"); // 路径相对于工作目录
}

void PlayScene::update(int deltaMs) {
    if (m_isUpgrading) return;
    if (m_gameOver) return;

    // 1. 玩家移动
    QPointF delta = m_moveDir * m_speed;
    movePlayer(delta);

    // 2. 更新所有对象（位置、动画等），但不处理碰撞
    updateGameObjects(deltaMs);   // 已移除内部碰撞调用

    // 3. 构建当前帧的网格（基于最新位置）
    updateGrid();

    // 4. 所有碰撞处理（使用最新网格）
    handleEnemyCollisionsUsingGrid();   // 敌人间碰撞
    handleExpOrbCollection();
    handleCollisionsWithBullets();      // 子弹-敌人
    handleBulletObstacleCollision();
    handlePlayerCollision();

    // 5. 清理死亡对象
    cleanupDeadObjects();

    // 6. 生成敌人
    static int spawnCounter = 0;
    spawnCounter++;
    if (spawnCounter > 15) {
        spawnCounter = 0;
        spawnEnemy();
    }
}

void PlayScene::setMoveDirection(const QPointF& dir) {
    m_moveDir = dir;
}

void PlayScene::loadObstacles(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open" << path;
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON";
        return;
    }
    QJsonObject root = doc.object();

    // 解析地图边界--防止玩家移出边界
    QJsonObject mapObj = root["map"].toObject();
    int mapWidth = mapObj["width"].toInt();
    int mapHeight = mapObj["height"].toInt();
    m_mapBounds = QRectF(0, 0, mapWidth, mapHeight);   //左上角为原点,然后设置地图边界
    qDebug() << "Map bounds:" << m_mapBounds;

    // 解析障碍物
    QJsonArray objects = root["collisionObjects"].toArray();
    for (const QJsonValue& val : objects) {
        QJsonObject obj = val.toObject();
        qreal x = obj["x"].toDouble();
        qreal y = obj["y"].toDouble();
        qreal w = obj["width"].toDouble();
        qreal h = obj["height"].toDouble();
        m_obstacles.append({QRectF(x, y, w, h)});
    }
    qDebug() << "Loaded" << m_obstacles.size() << "obstacles";
}

void PlayScene::movePlayer(const QPointF& delta) {
    QRectF newRect = m_playerRect.translated(delta);//用于计算移动后的坐标位置,来达到先检测后渲染的操作
    // 边界裁剪---如果角色移动方位导致移出边界,那么会对边界进行检测,如果移动出去了,那么就归为到原位置
    //通过计算边界和角色碰撞箱的位置来实现操作
    qreal minX = m_mapBounds.left();
    qreal maxX = m_mapBounds.right();//位置能移动最大是地图-玩家宽
    qreal minY = m_mapBounds.top();
    qreal maxY = m_mapBounds.bottom();//位置能移动最大是地图-玩家高
    if (newRect.left() < minX) newRect.moveLeft(minX);
    if (newRect.right() > maxX) newRect.moveRight(maxX);
    if (newRect.top() < minY) newRect.moveTop(minY);
    if (newRect.bottom() > maxY) newRect.moveBottom(maxY);


    //玩家和障碍物的碰撞检测----如果玩家的新闻只和障碍物进行碰撞后,那么坐标返回到原来的位置
    QRectF oldRect = m_playerRect;
    m_playerRect = newRect;
    if (collidesWithObstacles()) {
        m_playerRect = oldRect;
    }

 emit playerRectChanged();//更新坐标了,然后让qml进行更新操作


}

bool PlayScene::collidesWithObstacles() const {
    for (const Obstacle& obs : m_obstacles) {
        if (m_playerRect.intersects(obs.rect))//判断两个矩形是否相交
            return true;//遍历障碍物的坐标和碰撞位置是否碰撞,有一个就不能穿过
    }
    return false;
}

bool PlayScene::collidesWithObstacles(const QRectF& rect) const {
    for (const Obstacle& obs : m_obstacles) {
        if (rect.intersects(obs.rect))
            return true;
    }
    return false;
}




//角色移动相关
#include <QDebug>

void PlayScene::onKeyPress(Qt::Key key) {
    switch (key) {
    case Qt::Key_A:
        m_left = true;
        break;
    case Qt::Key_D:
        m_right = true;
        break;
    case Qt::Key_W:
        m_up = true;
        break;
    case Qt::Key_S:
        m_down = true;
        break;
    case Qt::Key_E:
        castSkill();
        break;
    case Qt::Key_Q:
        castMachineGun();
        break;
    default:
        return;
    }
    updateMovement();
}

void PlayScene::onKeyRelease(Qt::Key key) {
    switch (key) {
    case Qt::Key_A:
        m_left = false;
        break;
    case Qt::Key_D:
        m_right = false;
        break;
    case Qt::Key_W:
        m_up = false;
        break;
    case Qt::Key_S:
        m_down = false;
        break;
    default:
        return;
    }
    updateMovement();
}

void PlayScene::updateMovement() {
    float dx = (m_right ? 1.f : 0.f) - (m_left ? 1.f : 0.f);
    float dy = (m_down ? 1.f : 0.f) - (m_up ? 1.f : 0.f);
    if (dx != 0 && dy != 0) {
        float len = sqrt(dx*dx + dy*dy);
        dx /= len;
        dy /= len;
    }
    m_moveDir = QPointF(dx, dy);

    // 根据是否有移动输入来决定动画方向
    if (dx != 0 || dy != 0) {
        // 正在移动，更新动画方向
        if (dx > 0) m_animDir = AnimDir::Right;
        else if (dx < 0) m_animDir = AnimDir::Left;
        else if (dy > 0) m_animDir = AnimDir::Down;
        else if (dy < 0) m_animDir = AnimDir::Up;
        m_lastMoveDir = m_animDir;   // 记录最后一次移动方向
    } else {
        // 没有移动，使用最后一次移动方向（保持朝向）
        m_animDir = m_lastMoveDir;
    }
}


// 在 update 中，玩家与敌人碰撞
void PlayScene::handlePlayerCollision() {
    // 玩家缩小后的碰撞箱（宽高各一半，中心不变）
    QRectF playerCollisionRect(
        m_playerRect.center().x() - m_playerRect.width() / 4,
        m_playerRect.center().y() - m_playerRect.height() / 4,
        m_playerRect.width() / 2,
        m_playerRect.height() / 2
        );
    for (auto obj : m_objects) {
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            // 新增：如果敌人已死亡（正在播放死亡动画或已标记删除），则跳过
            if (!e->isAlive()) continue;
            // 敌人缩小后的碰撞箱
            QRectF enemyCollisionRect(
                e->rect().center().x() - e->rect().width() / 4,
                e->rect().center().y() - e->rect().height() / 4,
                e->rect().width() / 2,
                e->rect().height() / 2
                );
            if (!m_invincible &&playerCollisionRect.intersects(enemyCollisionRect)) {
                m_playerHp -= 500;
                emit playerHurt();
                emit playerHpChanged();   // 需要添加这行，否则血条不更新

                // 启动无敌帧
               startInvincible();  // 启动无敌闪烁


                if (m_playerHp <= 0) {
                     setGameOver(true);
                    return;
                }
                // 可选：将敌人弹开一段距离，避免连续碰撞
                break;
            }
        }
    }
}

// 处理子弹与敌人碰撞已在 Bullet::onCollision 中完成，但需要删除已死亡敌人和子弹
void PlayScene::cleanupDeadObjects() {
    for (int i = m_objects.size() - 1; i >= 0; --i) {
        GameObject* obj = m_objects[i];
        if (obj->isMarkedForDelete()) {
            delete obj;
            m_objects.removeAt(i);
            continue;
        }
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            if (e->isReadyToDelete()) {
                int expValue = 50 + m_level * 5;
                ExpOrb* orb = new ExpOrb(e->rect().center(), expValue, this);
                m_objects.append(orb);
                emit expOrbsChanged();
                delete e;
                m_objects.removeAt(i);
                continue;
            }
        }
        // 子弹超出边界的删除也统一处理（可选）
        if (Bullet* b = dynamic_cast<Bullet*>(obj)) {
            if (b->rect().x() < -100 || b->rect().x() > m_mapBounds.width()+100 ||
                b->rect().y() < -100 || b->rect().y() > m_mapBounds.height()+100) {
                b->markForDelete();  // 也可以直接删除，但为了统一，标记删除
            }
        }
    }
}








// ================= 敌人管理（基于 GameObject） =================



void PlayScene::spawnEnemy() {
    int side = QRandomGenerator::global()->bounded(4);
    qreal x, y;
    qreal w = m_mapBounds.width();
    qreal h = m_mapBounds.height();
    switch (side) {
    case 0: x = QRandomGenerator::global()->bounded((int)w); y = -40; break;
    case 1: x = w + 40; y = QRandomGenerator::global()->bounded((int)h); break;
    case 2: x = QRandomGenerator::global()->bounded((int)w); y = h + 40; break;
    default: x = -40; y = QRandomGenerator::global()->bounded((int)h); break;
    }
    QRectF startRect(x, y, 60, 60);
    PaimonEnemy* enemy = new PaimonEnemy(startRect, this, this);
    m_objects.append(enemy);
    // 发射信号通知 QML 敌人列表新增（但下一帧会统一发射，也可以立即发射）
    emit enemiesChanged();
}

void PlayScene::updateGameObjects(int deltaMs) {
    // 1. 更新所有对象
    for (auto obj : m_objects) {
        obj->update(deltaMs);
    }

    // 2. 敌人间碰撞
    // handleCollisions();

    // 3. 帧动画更新
    static int accumMs = 0;
    accumMs += deltaMs;
    if (accumMs >= 80) {
        accumMs -= 80;
        for (auto obj : m_objects) {
            if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                e->frameIndex = (e->frameIndex + 1) % 5;
            }
        }
    }

    // 4. 发射信号通知 QML 更新
    emit enemiesChanged();   // 敌人列表变化
    emit bulletsChanged();   // 子弹列表变化 ← 添加这一行
}

void PlayScene::handleCollisions() {
    // 敌人间碰撞使用网格优化
    handleEnemyCollisionsUsingGrid();
}

QVariantList PlayScene::enemies() const {
    QVariantList list;
    for (auto obj : m_objects) {
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            QVariantMap map;
            map["x"] = e->rect().x();
            map["y"] = e->rect().y();
            map["width"] = e->rect().width();
            map["height"] = e->rect().height();
            map["direction"] = e->direction;
            map["frameIndex"] = e->frameIndex;
            map["hp"] = e->hp;//展示血量
            map["isDying"] = e->isDying();//暴露死亡
            map["isFlashing"] = e->isFlashing();
            list.append(map);
        }
    }
    return list;
}

//////-----------------------------------------------子弹射击类


void PlayScene::shootBullet(const QPointF& target) {
    QPointF direction = target - m_playerRect.center();
    if (std::hypot(direction.x(), direction.y()) < 0.1f) return;
    Bullet* bullet = new Bullet(m_playerRect.center(), direction, this);
    // 设置穿透概率和次数
    bullet->setPenetrationChance(m_penetrationChance);
    int penetrationCount = (QRandomGenerator::global()->generateDouble() < m_penetrationChance) ? 2 : 0;
    bullet->setPenetrationCount(penetrationCount);
    m_objects.append(bullet);
}
void PlayScene::handleCollisionsWithBullets() {
    for (int i = 0; i < m_objects.size(); ++i) {
        Bullet* bullet = dynamic_cast<Bullet*>(m_objects[i]);
        if (!bullet || bullet->isMarkedForDelete()) continue;

        QPointF center = bullet->rect().center();
        int gx = static_cast<int>(center.x()) / GRID_SIZE;
        int gy = static_cast<int>(center.y()) / GRID_SIZE;

        bool hit = false;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                QPair<int,int> key(gx + dx, gy + dy);
                auto it = m_grid.find(key);
                if (it == m_grid.end()) continue;
                for (auto obj : it->objects) {
                    Enemy* enemy = dynamic_cast<Enemy*>(obj);
                    if (!enemy || !enemy->isAlive()) continue;
                    if (bullet->rect().intersects(enemy->rect())) {
                        enemy->takeDamage(m_bulletDamage);
                        enemy->triggerFlash(80);
                        if (enemy->hp <= 0 && !enemy->isDying()) {
                            enemy->startDeath(500);

                        }
                        emit explosionAt(bullet->rect().center().x(), bullet->rect().center().y());
                        QPointF dir = enemy->rect().center() - m_playerRect.center();
                        enemy->applyKnockback(dir, 5.0f);

                        if (bullet->canPenetrate()) {
                            bullet->usePenetration();
                            hit = false;   // 继续飞行
                            break;
                        } else {
                            bullet->markForDelete();   // 标记删除，但不立即删除
                            hit = true;
                            break;
                        }
                    }
                }
                if (hit) break;
            }
            if (hit) break;
        }
        // 子弹保留在 m_objects 中，由 cleanupDeadObjects 统一删除
    }
}

void PlayScene::handleBulletObstacleCollision() {
    for (int i = m_objects.size() - 1; i >= 0; --i) {
        Bullet* bullet = dynamic_cast<Bullet*>(m_objects[i]);
        if (!bullet) continue;
        if (collidesWithObstacles(bullet->rect())) {
            emit explosionAt(bullet->rect().center().x(), bullet->rect().center().y());
            bullet->markForDelete();      // 移除
        }
    }
}


QVariantList PlayScene::bullets() const {
    QVariantList list;
    for (auto obj : m_objects) {
        if (Bullet* b = dynamic_cast<Bullet*>(obj)) {
            QVariantMap map;
            map["x"] = b->rect().x();
            map["y"] = b->rect().y();
            list.append(map);
        }
    }
    return list;
}




//-----------------------------------------------升级相关-----------------------------------------
void PlayScene::handleExpOrbCollection() {
    for (int i = m_objects.size() - 1; i >= 0; --i) {
        ExpOrb* orb = dynamic_cast<ExpOrb*>(m_objects[i]);
        if (!orb) continue;

        QPointF toPlayer = m_playerRect.center() - orb->rect().center();
        if (toPlayer.manhattanLength() < 100) { // 拾取半径
            orb->setTarget(m_playerRect.center());

        }

       if (orb->isMovingToPlayer() && (orb->rect().center() - m_playerRect.center()).manhattanLength() < 15) {
            addExp(orb->value());//升级
            delete orb;
            m_objects.removeAt(i);
             emit expOrbsChanged();   // 添加这一行，通知 QML 刷新
        }
    }
}


void PlayScene::addExp(int value) {

    //经验达到加血效果--超出最大血量.按最大血量算
    if((m_playerHp+=100)<=m_maxHp){}
    else{m_playerHp=m_maxHp;}
    emit playerHpChanged();



    if (m_level >= 20) {
        // 满级后不再增加经验，可丢弃经验球（或显示提示）
        return;
    }


    m_currentExp += value;
    while (m_currentExp >= m_expToNextLevel && m_level < 20) {
        m_currentExp -= m_expToNextLevel;
        m_level++;
        m_expToNextLevel = 100 + m_level * 20;
        upgradeLevel(); // 应用升级增益（弹出选择界面）
        emit statsChanged();
    }
    emit statsChanged();
}

void PlayScene::upgradeLevel() {
     setUpgrading(true);
    QStringList options = generateUpgradeOptions();
    emit upgradeRequested(options);
     m_currentUpgradeOptions = options;
    // 注意：游戏需要暂停等待玩家选择，通过 QML 弹窗后调用 applyUpgrade
}

QStringList PlayScene::generateUpgradeOptions() {
    QStringList allOptions = {
        "增加最大生命值 +100",
        "增加子弹伤害 +100",
        "增加移动速度 +1",
        "增加子弹穿透概率 +10% (最高90%)"
    };
    // 随机取三个不重复的
    QStringList chosen;
    QList<int> indices = {0,1,2,3};
    std::random_shuffle(indices.begin(), indices.end());
    for (int i = 0; i < 3; ++i) chosen << allOptions[indices[i]];
    return chosen;
}

void PlayScene::applyUpgrade(int index) {
    QStringList options = m_currentUpgradeOptions; // 实际应保存上一次生成的选项
    QString chosen = options[index];
    if (chosen.contains("最大生命值")) {
        m_maxHp += 100;
        m_playerHp = m_maxHp;
        emit playerHpChanged();
    } else if (chosen.contains("子弹伤害")) {
        m_bulletDamage += 100;
    } else if (chosen.contains("移动速度")) {
        m_speed += 1;
    } else if (chosen.contains("穿透概率")) {
        m_penetrationChance += 0.1f;
        if (m_penetrationChance > 0.9f) m_penetrationChance = 0.9f;
    }
    emit statsChanged();
     setUpgrading(false);   // 恢复游戏主循环（需要外部实现暂停标志）
}


QVariantList PlayScene::expOrbs() const {
    QVariantList list;
    for (auto obj : m_objects) {
        if (ExpOrb* orb = dynamic_cast<ExpOrb*>(obj)) {
            QVariantMap map;
            map["x"] = orb->rect().x();
            map["y"] = orb->rect().y();
            list.append(map);
        }
    }
    return list;
}



#include <QApplication>
#include <QCursor>

void PlayScene::setUpgrading(bool upgrading) {
    if (m_isUpgrading == upgrading) return;
    m_isUpgrading = upgrading;
    if (m_isUpgrading) {
        // 升级弹窗时恢复鼠标光标（因为全局隐藏了）
        QApplication::restoreOverrideCursor();
    } else {
        // 恢复游戏时重新隐藏光标
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
    }
}


void PlayScene::startInvincible() {
    if (m_invincible) return;
    m_invincible = true;
    emit invincibleChanged(true);
    m_invincibleTimer->start(2000);  // 2秒无敌
}

//技能散射

#include <cmath>   // 已包含

void PlayScene::castSkill() {

    if(!m_skillReady)
        return;//没冷却好退出

    emit skillCast();

    const float angleStepRad = m_skillAngleStep * M_PI / 180.0f;
    const int bulletCount = static_cast<int>(360.0f / m_skillAngleStep);
    QPointF center = m_playerRect.center();



    for (int i = 0; i < bulletCount; ++i) {
        float angle = i * angleStepRad;
        QPointF dir(std::cos(angle), std::sin(angle));
        // 创建子弹，方向为 dir，起始位置为玩家中心
        Bullet* bullet = new Bullet(center, dir, this);
        bullet->setPenetrationChance(m_penetrationChance);
        int penetrationCount = (QRandomGenerator::global()->generateDouble() < m_penetrationChance) ? 2 : 0;
        bullet->setPenetrationCount(penetrationCount);
        m_objects.append(bullet);
        // 可设置技能子弹的特殊属性（如速度、伤害、穿透等），但这里使用默认
    }


    // 进入冷却
    m_skillReady = false;
    m_skillCooldownRemaining = m_skillCooldownMs;
    m_skillCooldownTimer->start();
    emit skillReadyChanged();
    emit skillCooldownUpdated();

}


//技能实现机枪
void PlayScene::fireOneBulletTowardsDirection() {
    // 获取角色朝向方向
    QPointF dir = m_moveDir;
    if (dir.manhattanLength() < 0.01f) {
        // 静止时使用最后一次移动方向
        switch (m_lastMoveDir) {
        case AnimDir::Right: dir = QPointF(1, 0); break;
        case AnimDir::Left:  dir = QPointF(-1, 0); break;
        case AnimDir::Up:    dir = QPointF(0, -1); break;
        case AnimDir::Down:  dir = QPointF(0, 1); break;
        default: dir = QPointF(1, 0);
        }
    }
    float len = std::hypot(dir.x(), dir.y());
    if (len > 0) dir /= len;

    // 计算扇形角度范围（弧度）
    float halfAngle = (m_machineGunSpreadAngle / 2.0f) * M_PI / 180.0f;
    int count = m_machineGunBulletsPerBurst;
    QPointF center = m_playerRect.center();

    for (int i = 0; i < count; ++i) {
        // 在 -halfAngle 到 +halfAngle 之间均匀分布
        float t = (count > 1) ? (float)i / (count - 1) : 0.5f; // 0~1
        float angle = -halfAngle + t * 2.0f * halfAngle;
        // 旋转方向向量
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        QPointF rotatedDir(dir.x() * cosA - dir.y() * sinA,
                           dir.x() * sinA + dir.y() * cosA);
        // 发射子弹--设置相同的属性
        Bullet* bullet = new Bullet(center, rotatedDir, this);
        bullet->setPenetrationChance(m_penetrationChance);
        int penetrationCount = (QRandomGenerator::global()->generateDouble() < m_penetrationChance) ? 2 : 0;
        bullet->setPenetrationCount(penetrationCount);
        m_objects.append(bullet);
    }
}


void PlayScene::castMachineGun() {
    if (!m_machineGunReady || m_machineGunActive) return;

     emit machineGunCast();   // 发射信号
    m_machineGunActive = true;
    m_machineGunReady = false;
    emit machineGunReadyChanged();

    // 启动持续计时器（2秒后结束）
    m_machineGunTimer->start(m_machineGunDuration);
    // 启动发射定时器（每50ms一发）
    m_machineGunFireTimer->start();
    // 启动冷却计时器（3秒后技能恢复）
    m_machineGunCooldownTimer->start(m_machineGunCooldown);
    // 启动冷却更新定时器
    m_machineGunCooldownUpdateTimer->start();
}

int PlayScene::machineGunCooldownRemaining() const {
    if (m_machineGunCooldownTimer->isActive()) {
        return m_machineGunCooldownTimer->remainingTime();
    }
    return 0;
}



void PlayScene::resetGame() {
    // 1. 重置玩家状态
    m_playerRect = QRectF(100, 100, 80, 80);
    m_playerHp = 1000;
    m_maxHp = 1000;
    m_speed = 5.0f;
    m_bulletDamage = 100;
    m_penetrationChance = 0.0f;

    // 2. 重置升级相关
    m_level = 1;
    m_currentExp = 0;
    m_expToNextLevel = 120;
    m_isUpgrading = false;

    // 3. 重置技能冷却
    m_skillReady = true;
    m_skillCooldownRemaining = 0;
    if (m_skillCooldownTimer) m_skillCooldownTimer->stop();

    m_machineGunReady = true;
    m_machineGunActive = false;
    if (m_machineGunTimer) m_machineGunTimer->stop();
    if (m_machineGunFireTimer) m_machineGunFireTimer->stop();
    if (m_machineGunCooldownTimer) m_machineGunCooldownTimer->stop();
    if (m_machineGunCooldownUpdateTimer) m_machineGunCooldownUpdateTimer->stop();

    // 4. 清除所有游戏对象（敌人、子弹、经验球等）
    for (auto obj : m_objects) {
        delete obj;
    }
    m_objects.clear();

    // 5. 重置无敌状态
    m_invincible = false;
    emit invincibleChanged(false);

    // 6. 重置移动方向
    m_moveDir = QPointF(0,0);
    m_animDir = AnimDir::Right;
    m_lastMoveDir = AnimDir::Right;
    m_left = m_right = m_up = m_down = false;

    // 7. 发射信号通知 UI 更新
    emit playerRectChanged();
    emit playerHpChanged();
    emit statsChanged();
    emit enemiesChanged();
    emit bulletsChanged();
    emit expOrbsChanged();
    emit skillReadyChanged();
    emit skillCooldownUpdated();
    emit machineGunReadyChanged();
    emit machineGunCooldownUpdated();

    setGameOver(false);
    qDebug() << "PlayScene reset";
}



void PlayScene::setGameOver(bool over) {
    if (m_gameOver == over) return;
    m_gameOver = over;
    emit gameOverChanged();
}

#include <QApplication>

void PlayScene::quitGame() {
    QApplication::quit();
}



void PlayScene::updateGrid() {
    m_grid.clear();
    for (auto obj : m_objects) {
        // 只处理敌人（type=1）和子弹（type=2）
        int type = obj->type();
        if (type != 1 && type != 2) continue;
        QPointF center = obj->rect().center();
        int gx = static_cast<int>(center.x()) / GRID_SIZE;
        int gy = static_cast<int>(center.y()) / GRID_SIZE;
        QPair<int,int> key(gx, gy);
        m_grid[key].objects.append(obj);
    }
}


void PlayScene::handleEnemyCollisionsUsingGrid() {
    // 获取所有敌人的列表
    QList<Enemy*> enemies;
    for (auto obj : m_objects) {
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            enemies.append(e);
        }
    }
    if (enemies.size() < 2) return;

    // 构建网格（如果之前没有构建，可以调用 updateGrid，但 updateGrid 已经每帧调用）
    // 我们直接使用 m_grid，但它包含子弹，我们需要过滤出敌人
    // 为了效率，可以专门构建敌人网格，但简单起见，我们遍历网格并过滤敌人

    // 遍历所有网格，对每个网格内的敌人进行碰撞
    // 为了避免重复检测，我们只检测同一网格内的敌人，以及网格与右、下、右下相邻网格的敌人
    // 但网格的 key 是 (gx, gy)，我们遍历所有 key

    QList<QPair<int,int>> keys = m_grid.keys();
    for (const auto& key : keys) {
        int gx = key.first;
        int gy = key.second;

        // 当前网格中的敌人
        QList<Enemy*> gridEnemies;
        for (auto obj : m_grid[key].objects) {
            if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                gridEnemies.append(e);
            }
        }
        // 1. 当前网格内敌人两两碰撞
        for (int i = 0; i < gridEnemies.size(); ++i) {
            for (int j = i+1; j < gridEnemies.size(); ++j) {
                resolveEnemyCollision(gridEnemies[i], gridEnemies[j]);
            }
        }

        // 2. 与右侧相邻网格 (gx+1, gy) 的敌人碰撞
        QPair<int,int> rightKey(gx+1, gy);
        if (m_grid.contains(rightKey)) {
            QList<Enemy*> rightEnemies;
            for (auto obj : m_grid[rightKey].objects) {
                if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                    rightEnemies.append(e);
                }
            }
            for (auto* e1 : gridEnemies) {
                for (auto* e2 : rightEnemies) {
                    resolveEnemyCollision(e1, e2);
                }
            }
        }

        // 3. 与下方相邻网格 (gx, gy+1) 的敌人碰撞
        QPair<int,int> downKey(gx, gy+1);
        if (m_grid.contains(downKey)) {
            QList<Enemy*> downEnemies;
            for (auto obj : m_grid[downKey].objects) {
                if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                    downEnemies.append(e);
                }
            }
            for (auto* e1 : gridEnemies) {
                for (auto* e2 : downEnemies) {
                    resolveEnemyCollision(e1, e2);
                }
            }
        }

        // 4. 与右下相邻网格 (gx+1, gy+1) 的敌人碰撞
        QPair<int,int> downRightKey(gx+1, gy+1);
        if (m_grid.contains(downRightKey)) {
            QList<Enemy*> downRightEnemies;
            for (auto obj : m_grid[downRightKey].objects) {
                if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                    downRightEnemies.append(e);
                }
            }
            for (auto* e1 : gridEnemies) {
                for (auto* e2 : downRightEnemies) {
                    resolveEnemyCollision(e1, e2);
                }
            }
        }
    }
}


void PlayScene::resolveEnemyCollision(Enemy* e1, Enemy* e2) {
    if (!e1 || !e2) return;
    if (!e1->isAlive() || !e2->isAlive()) return;

    QRectF r1 = e1->rect();
    QRectF r2 = e2->rect();
    QRectF phy1(r1.center().x() - r1.width()/4,
                r1.center().y() - r1.height()/4,
                r1.width()/2, r1.height()/2);
    QRectF phy2(r2.center().x() - r2.width()/4,
                r2.center().y() - r2.height()/4,
                r2.width()/2, r2.height()/2);
    if (phy1.intersects(phy2)) {
        QPointF diff = r1.center() - r2.center();
        if (diff.x() == 0 && diff.y() == 0) diff = QPointF(1,0);
        qreal len = sqrt(diff.x()*diff.x() + diff.y()*diff.y());
        diff /= len;
        qreal overlap = (phy1.width()/2 + phy2.width()/2) -
                        QLineF(phy1.center(), phy2.center()).length();
        if (overlap > 0) {
            r1.translate(diff * overlap / 2);
            r2.translate(-diff * overlap / 2);
            e1->setRect(r1);
            e2->setRect(r2);
        }
    }
}
