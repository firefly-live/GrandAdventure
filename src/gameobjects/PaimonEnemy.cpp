#include "PaimonEnemy.h"
#include "../scenes/PlayScene.h"
#include <cmath>

PaimonEnemy::PaimonEnemy(const QRectF& startRect, PlayScene* scene, QObject *parent)
    : Enemy(parent), m_scene(scene) {
    m_rect = startRect;
    speed = 1.0f;
    direction = 2;
    frameIndex = 0;
}

void PaimonEnemy::update(int deltaMs) {
    updateDeath(deltaMs);
    if (m_isDying || m_readyToDelete) {
        // 死亡状态不移动
        return;
    }

    updateFlash(deltaMs);//闪白
    //  应用击退
    if (hasKnockback()) {
        m_rect.translate(m_knockbackVelocity);
        m_knockbackVelocity *= m_knockbackDecay; // 每帧衰减
        if (m_knockbackVelocity.manhattanLength() < 0.1f) {
            m_knockbackVelocity = QPointF(0,0);
        }
        // 击退期间不执行追踪逻辑（避免抵消击退效果）
        return;
    }



    Q_UNUSED(deltaMs);
    if (!m_scene) return;

    QRectF playerRect = m_scene->playerRect();
    QPointF dir = playerRect.center() - m_rect.center();
    float len = sqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (len > 0) dir /= len;
    QPointF delta = dir * speed;
    QRectF newRect = m_rect.translated(delta);

    // 边界限制（需获取地图边界，可以在 PlayScene 中添加 getMapBounds 方法）
    QRectF mapBounds = m_scene->mapBounds(); // 需要在 PlayScene 中添加 getter
    qreal minX = mapBounds.left();
    qreal maxX = mapBounds.right() - m_rect.width();
    qreal minY = mapBounds.top();
    qreal maxY = mapBounds.bottom() - m_rect.height();
    if (newRect.left() < minX) newRect.moveLeft(minX);
    if (newRect.right() > maxX) newRect.moveRight(maxX);
    if (newRect.top() < minY) newRect.moveTop(minY);
    if (newRect.bottom() > maxY) newRect.moveBottom(maxY);

    // 障碍物碰撞检测
    if (!m_scene->collidesWithObstacles(newRect)) {
        m_rect = newRect;
        emit rectChanged();
    }

    // 更新方向
    if (m_rect.center().x() < playerRect.center().x())
        direction = 2;
    else
        direction = 1;
}
