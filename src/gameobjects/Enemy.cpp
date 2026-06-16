#include "Enemy.h"
Enemy::Enemy(QObject *parent) : GameObject(parent) {}
void Enemy::setRect(const QRectF& r) { m_rect = r; emit rectChanged(); }
void Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {
            markForDelete();   // 仅标记，不删除
    }
}
void Enemy::startDeath(int durationMs) {
    if (m_isDying) return;
    m_isDying = true;
    m_readyToDelete = false;
    m_deathTimer = durationMs;
    emit rectChanged(); // 通知 QML 状态变化（如果有绑定）
}

void Enemy::updateDeath(int deltaMs) {
    if (!m_isDying) return;
    m_deathTimer -= deltaMs;
    if (m_deathTimer <= 0) {
        m_isDying = false;
        m_readyToDelete = true;
        // 标记删除，由 PlayScene 清理
    }
}
