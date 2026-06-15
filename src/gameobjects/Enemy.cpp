#include "Enemy.h"
Enemy::Enemy(QObject *parent) : GameObject(parent) {}
void Enemy::setRect(const QRectF& r) { m_rect = r; emit rectChanged(); }
void Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
            markForDelete();   // 仅标记，不删除
    }
}
