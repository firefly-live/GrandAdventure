#include "PlayScene.h"
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
    m_playerRect = QRectF(100, 100, 80, 80);
}

void PlayScene::onEnter() {
    qDebug() << "PlayScene entered";
    loadObstacles("../../Resource/bgc/obstacles.json"); // 路径相对于工作目录
}

void PlayScene::update(int deltaMs) {
    // 1. 玩家移动
    QPointF delta = m_moveDir * m_speed;
    movePlayer(delta);

    // 2. 更新所有游戏对象（敌人）
    updateGameObjects(deltaMs);

    // 3. 生成新敌人（每60帧一次）
    static int spawnCounter = 0;
    spawnCounter++;
    if (spawnCounter > 60) {
        spawnCounter = 0;
        spawnEnemy();
    }

    // 4. 通知QML敌人列表变化（已在 updateGameObjects 中发射，但为了位置变化也发射，可在此再次发射）
    //    实际在 updateGameObjects 的帧动画部分已经发射，位置变化时如需刷新可在 movePlayer 后发射？但敌人位置变化由 update 推动，无需额外发射。
    //    这里保持简洁，不移除原有逻辑但需要适配新信号
    //    原代码有 emit enemiesChanged()，现在改为 emit gameObjectsChanged()，但注意频繁发射可能影响性能，可仅在敌人增删或帧变化时发射。
    //    下面会在 updateGameObjects 中处理帧变化时发射，位置变化不额外发射。
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











// ================= 敌人管理（基于 GameObject） =================

void PlayScene::spawnEnemy() {
    // 计算生成位置
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
    // 创建 PaimonEnemy，传入 this 以便访问障碍物和边界
    PaimonEnemy* enemy = new PaimonEnemy(startRect, this, this);
    m_objects.append(enemy);
    emit gameObjectsChanged();
}

void PlayScene::updateGameObjects(int deltaMs) {
    // 先更新所有对象的位置和逻辑
    for (auto obj : m_objects) {
        obj->update(deltaMs);
    }

    // 处理碰撞（敌人之间互相推开，敌人与障碍物已在 update 中处理）
    handleCollisions();

    // 统一更新帧动画（每80ms切换）
    static int accumMs = 0;
    accumMs += deltaMs;
    if (accumMs >= 80) {
        accumMs -= 80;
        for (auto obj : m_objects) {
            if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
                e->frameIndex = (e->frameIndex + 1) % 5;
            }
        }
        emit gameObjectsChanged();  // 帧变化，通知 QML 刷新
    }
}

void PlayScene::handleCollisions() {
    // 敌人之间互相推开（基于物理碰撞箱，与旧逻辑相同）
    QList<Enemy*> enemies;
    for (auto obj : m_objects) {
        if (Enemy* e = dynamic_cast<Enemy*>(obj)) {
            enemies.append(e);
        }
    }
    for (int i = 0; i < enemies.size(); ++i) {
        for (int j = i+1; j < enemies.size(); ++j) {
            QRectF r1 = enemies[i]->rect();
            QRectF r2 = enemies[j]->rect();
            // 物理碰撞箱（大小一半）
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

QVariantList PlayScene::gameObjects() const {
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
            list.append(map);
        }
    }
    return list;
}
