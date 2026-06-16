#include "Bullet.h"
#include "Enemy.h"
#include <cmath>

Bullet::Bullet(const QPointF& startPos, const QPointF& direction, QObject* parent)
    : GameObject(parent) {

     // 子弹矩形：10x10，中心对齐起始点
    m_rect = QRectF(startPos.x() - 5, startPos.y() - 5, 10, 10);
    float len = std::hypot(direction.x(), direction.y());
    if (len > 0) {
        QPointF dir = direction / len;
        m_velocity = dir * m_speed;
    } else {
        m_velocity = QPointF(0,0);
    }
}

void Bullet::update(int deltaMs) {
    float dt = deltaMs / 1000.0f;          // 转换为秒
    m_rect.translate(m_velocity * dt);     // 速度（像素/秒）× 时间（秒）
    emit rectChanged();
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


int  Bullet::takeDamage(){
    int damage = m_damage;
    return damage;
}
