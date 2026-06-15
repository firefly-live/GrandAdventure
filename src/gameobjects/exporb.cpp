#include "ExpOrb.h"
#include <cmath>

ExpOrb::ExpOrb(const QPointF& pos, int value, QObject* parent)
    : GameObject(parent), m_value(value) {
    m_rect = QRectF(pos.x() - 10, pos.y() - 10, 20, 20);
}

void ExpOrb::setTarget(const QPointF& target) {
    m_target = target;
    m_movingToPlayer = true;
}

void ExpOrb::update(int deltaMs) {
    if (!m_movingToPlayer) return;
    float dt = deltaMs / 1000.0f;
    QPointF dir = m_target - m_rect.center();
    float len = std::hypot(dir.x(), dir.y());
    if (len < 5.0f) {
        m_movingToPlayer = false;
        return;
    }
    dir /= len;
    QPointF delta = dir * m_speed * dt;
    m_rect.translate(delta);
    emit rectChanged();
}
