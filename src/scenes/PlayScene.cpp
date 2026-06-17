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






    m_playerRect = QRectF(100, 100, 80, 80);
}

void PlayScene::onEnter() {
    qDebug() << "PlayScene entered";
    loadObstacles("../../Resource/bgc/obstacles.json"); // 路径相对于工作目录
}

void PlayScene::update(int deltaMs) {
     if (m_isUpgrading) return;  // 升级中，不更新任何游戏逻辑
    // 1. 玩家移动
    QPointF delta = m_moveDir * m_speed;
    movePlayer(delta);

    //更新敌人操作
    updateGameObjects(deltaMs);   // 包含了敌人更新、碰撞、帧动画和发射信号
    handleExpOrbCollection();
    //更新碰撞操作
    // 处理子弹与敌人碰撞
    handleCollisionsWithBullets();
    // 处理子弹与障碍物碰撞
    handleBulletObstacleCollision();
    // 玩家与敌人碰撞（已有）
    handlePlayerCollision();
    // 清理死亡对象
    cleanupDeadObjects();


    static int spawnCounter = 0;
    spawnCounter++;
    if (spawnCounter > 60) {
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
#include <QDebug>  // 需要添加这个头文件

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
    case Qt::Key_F:
        castSkill();
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
                m_playerHp -= 1000;
                emit playerHpChanged();   // 需要添加这行，否则血条不更新

                // 启动无敌帧
               startInvincible();  // 启动无敌闪烁


                if (m_playerHp <= 0) {
                    SceneManager::instance()->switchTo(SceneType::Death);//由于没有这个,所以无效
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
    for (int i = m_objects.size()-1; i >= 0; --i) {
        GameObject* obj = m_objects[i];
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            if (e->isReadyToDelete()) {
                // 生成经验球
                int expValue = 50 + m_level * 5;
                ExpOrb* orb = new ExpOrb(e->rect().center(), expValue, this);
                m_objects.append(orb);
                emit expOrbsChanged();
                delete e;
                m_objects.removeAt(i);
                continue;
            }
        }
        // 子弹已在碰撞中 deleteLater 移除，此处可额外处理超出边界的子弹
        if (Bullet* b = dynamic_cast<Bullet*>(obj)) {
            if (b->rect().x() < -100 || b->rect().x() > m_mapBounds.width()+100 ||
                b->rect().y() < -100 || b->rect().y() > m_mapBounds.height()+100) {
                b->deleteLater();
                m_objects.removeAt(i);
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
    handleCollisions();

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
    // 收集所有敌人
    QList<Enemy*> enemies;
    for (auto obj : m_objects) {
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            enemies.append(e);
        }
    }
    // 两两碰撞推开（与原有逻辑相同）
    for (int i = 0; i < enemies.size(); ++i) {
        for (int j = i+1; j < enemies.size(); ++j) {
            QRectF r1 = enemies[i]->rect();
            QRectF r2 = enemies[j]->rect();
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
                    enemies[i]->setRect(r1);
                    enemies[j]->setRect(r2);
                }
            }
        }
    }
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
        if (!bullet) continue;

        bool hit = false;
        for (int j = 0; j < m_objects.size(); ++j) {
            Enemy* enemy = dynamic_cast<Enemy*>(m_objects[j]);
            if (!enemy) continue;
            // 跳过已死亡（正在播放死亡动画或已标记删除）的敌人
            if (!enemy->isAlive()) continue;

            if (bullet->rect().intersects(enemy->rect())) {
                enemy->takeDamage(m_bulletDamage);

                 enemy->triggerFlash(80);   // 触发闪白，持续80ms

                if (enemy->hp <= 0 && !enemy->isDying()) {
                    enemy->startDeath(500);   // 启动死亡闪烁
                }

                emit explosionAt(bullet->rect().center().x(), bullet->rect().center().y());    // 发射爆炸信号动画
                hit = true;
                // 穿透逻辑
                // 击退方向：从玩家指向敌人
                QPointF dir = enemy->rect().center() - m_playerRect.center();
                float force = 5.0f;  // 基础击退力度（可升级调整）
                enemy->applyKnockback(dir, force);


                if (bullet->canPenetrate()) {
                    bullet->usePenetration();   // 消耗一次穿透次数
                    // 继续飞行，不删除子弹，跳出内层循环检查下一个敌人（同一帧可能击中多个）
                    // 注意：这里不 break，因为同一颗子弹可能需要穿透后继续检测同一帧的其他敌人
                    // 但为简单起见，每帧只处理一次击中，后续帧会继续检测
                    break;
                } else {
                    // 无法穿透，标记删除
                    delete bullet;
                    m_objects.removeAt(i);
                    i--; // 调整索引
                    break;
                }



            }
        }
        // 注意：若穿透且击中了敌人，子弹未删除，继续后续循环
    }
}

void PlayScene::handleBulletObstacleCollision() {
    for (int i = m_objects.size() - 1; i >= 0; --i) {
        Bullet* bullet = dynamic_cast<Bullet*>(m_objects[i]);
        if (!bullet) continue;
        if (collidesWithObstacles(bullet->rect())) {
            emit explosionAt(bullet->rect().center().x(), bullet->rect().center().y());    // 发射爆炸信号动画
            delete bullet;               // 直接删除
            m_objects.removeAt(i);       // 移除
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

    const float angleStepRad = m_skillAngleStep * M_PI / 180.0f;
    const int bulletCount = static_cast<int>(360.0f / m_skillAngleStep);
    QPointF center = m_playerRect.center();

    for (int i = 0; i < bulletCount; ++i) {
        float angle = i * angleStepRad;
        QPointF dir(std::cos(angle), std::sin(angle));
        // 创建子弹，方向为 dir，起始位置为玩家中心
        Bullet* bullet = new Bullet(center, dir, this);
        // 可设置技能子弹的特殊属性（如速度、伤害、穿透等），但这里使用默认
        // bullet->setSpeed(技能速度);
        m_objects.append(bullet);
    }


    // 进入冷却
    m_skillReady = false;
    m_skillCooldownRemaining = m_skillCooldownMs;
    m_skillCooldownTimer->start();
    emit skillReadyChanged();
    emit skillCooldownUpdated();
    // 可选：发射后发射一个声音或特效，但不强制
}
