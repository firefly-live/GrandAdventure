#include "Enemy.h"
Enemy::Enemy(QObject *parent) : GameObject(parent) {}
void Enemy::setRect(const QRectF& r) { m_rect = r; emit rectChanged(); }
void Enemy::takeDamage(int damage) {
    hp -= damage;
    if (hp <= 0) {

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

void Enemy::applyKnockback(const QPointF& direction, float force) {
    // 方向归一化，乘以力度
    float len = std::hypot(direction.x(), direction.y());
    if (len > 0) {
        m_knockbackVelocity = (direction / len) * force;
    } else {
        m_knockbackVelocity = QPointF(0,0);
    }
}


void Enemy::triggerFlash(int durationMs) {
    m_whiteFlash = true;
    m_flashTimer = durationMs;
}

// 在 Enemy::updateDeath 或子类 update 中调用
void Enemy::updateFlash(int deltaMs) {
    if (!m_whiteFlash) return;
    m_flashTimer -= deltaMs;
    if (m_flashTimer <= 0) {
        m_whiteFlash = false;
        emit rectChanged(); // 通知 QML 更新
    }
}
