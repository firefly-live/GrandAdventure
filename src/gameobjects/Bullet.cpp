#include "Bullet.h"
#include "Enemy.h"
#include <cmath>

Bullet::Bullet(const QPointF& startPos, const QPointF& direction, QObject* parent)
    : GameObject(parent) {
    m_rect = QRectF(startPos.x(), startPos.y(), 10, 10);
    // 归一化方向
    float len = sqrt(direction.x()*direction.x() + direction.y()*direction.y());
    if (len > 0) {
        m_velocity = direction / len * m_speed;
    } else {
        m_velocity = QPointF(0,0);
    }
}

void Bullet::update(int deltaMs) {
    Q_UNUSED(deltaMs);
    m_rect.translate(m_velocity);
    // 超出边界则标记删除（由 PlayScene 处理）
    // 简单超出地图边界销毁（可后续添加）
}

void Bullet::onCollision(GameObject* other) {
    if (other->type() == 1) { // 敌人
        Enemy* enemy = dynamic_cast<Enemy*>(other);
        if (enemy) {
            enemy->takeDamage(m_damage);
            // 子弹自身标记删除（将在 PlayScene 中处理）
            delete this;
        }
    }
}
