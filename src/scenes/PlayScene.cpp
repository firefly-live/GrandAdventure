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

    Q_UNUSED(deltaMs);
    QPointF delta = m_moveDir * m_speed;  // m_speed = 5.f
    movePlayer(delta);


    //--------------------------------------------------敌人类

    // 更新敌人
    updateEnemies(deltaMs);

    // 生成敌人（每60帧约1秒生成一个）
    m_enemySpawnCounter++;
    if (m_enemySpawnCounter > 60) {
        m_enemySpawnCounter = 0;
        spawnEnemy();
    }

    emit enemiesChanged(); // 通知 QML 刷新敌人列表

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











////--------------------------------------------------敌人类
void PlayScene::spawnEnemy() {
    Enemy e;
    e.speed = m_enemySpeed;
    // 在地图边缘外生成（稍微偏移）
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
    e.rect = QRectF(x, y, 60, 60);  // 碰撞箱比玩家小（60x60）
    e.direction = 2; // 默认右
    e.frameIndex = 0;   // 从第0帧开始
    m_enemies.append(e);

}


void PlayScene::updateEnemies(int deltaMs) {
    Q_UNUSED(deltaMs);
    for (auto& e : m_enemies) {
        // 计算朝向玩家的方向向量
        QPointF dir = m_playerRect.center() - e.rect.center();
        qreal len = sqrt(dir.x()*dir.x() + dir.y()*dir.y());
        if (len > 0) dir /= len;
        QPointF delta = dir * e.speed;
        QRectF newRect = e.rect.translated(delta);

        // 边界限制（不能移出地图）
        qreal minX = m_mapBounds.left();
        qreal maxX = m_mapBounds.right() - e.rect.width();
        qreal minY = m_mapBounds.top();
        qreal maxY = m_mapBounds.bottom() - e.rect.height();
        if (newRect.left() < minX) newRect.moveLeft(minX);
        if (newRect.right() > maxX) newRect.moveRight(maxX);
        if (newRect.top() < minY) newRect.moveTop(minY);
        if (newRect.bottom() > maxY) newRect.moveBottom(maxY);
        e.rect = newRect;

        // 确定敌人的图片方向（面向玩家）
        // 如果敌人在玩家左边，它应该朝右（direction=2）；在右边朝左（direction=1）
        if (e.rect.center().x() < m_playerRect.center().x()) {
            e.direction = 2; // 朝右
        } else {
            e.direction = 1; // 朝左
        }
    }

    // 简单的敌人间碰撞避免（互相推开）
    for (int i = 0; i < m_enemies.size(); ++i) {
        for (int j = i+1; j < m_enemies.size(); ++j) {
            QRectF& r1 = m_enemies[i].rect;
            QRectF& r2 = m_enemies[j].rect;
            if (r1.intersects(r2)) {
                QPointF diff = r1.center() - r2.center();
                if (diff.x() == 0 && diff.y() == 0) diff = QPointF(1,0);
                qreal len = sqrt(diff.x()*diff.x() + diff.y()*diff.y());
                if (len == 0) continue;
                diff /= len;
                qreal overlap = (r1.width()/2 + r2.width()/2) - QLineF(r1.center(), r2.center()).length();
                if (overlap > 0) {
                    r1.translate(diff * overlap/2);
                    r2.translate(-diff * overlap/2);
                }
            }
        }
    }
    static int accumMs = 0;
    accumMs += deltaMs;
    if (accumMs >= 80) {   // 每80ms切换一次帧
        accumMs -= 80;
        for (auto& e : m_enemies) {
            e.frameIndex = (e.frameIndex + 1) % 5;   // 5帧
        }
        emit enemiesChanged();   // 通知QML刷新（因为帧索引变化）
    }


}


QVariantList PlayScene::enemies() const {
    m_enemiesCache.clear();
    for (const Enemy& e : m_enemies) {
        QVariantMap map;
        map["x"] = e.rect.x();
        map["y"] = e.rect.y();
        map["width"] = e.rect.width();
        map["height"] = e.rect.height();
        map["direction"] = e.direction;
        map["frameIndex"] = e.frameIndex;
        m_enemiesCache.append(map);
    }
    return m_enemiesCache;
}
