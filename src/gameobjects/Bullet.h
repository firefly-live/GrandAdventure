#pragma once
#include "../core/GameObject.h"

class Bullet : public GameObject {
    Q_OBJECT
public:
    Bullet(const QPointF& startPos, const QPointF& direction, QObject* parent = nullptr);
    QRectF rect() const override { return m_rect; }
    int type() const override { return 2; }
    void update(int deltaMs) override;
    void onCollision(GameObject* other) override;
    int takeDamage();

    void setPenetrationChance(float chance) { m_penetrationChance = chance; }
    void setPenetrationCount(int count) { m_remainingPenetrations = count; }
    bool canPenetrate() const { return m_remainingPenetrations > 0; }
    void usePenetration() { if (m_remainingPenetrations > 0) m_remainingPenetrations--; }

private:
    QRectF m_rect;
    QPointF m_velocity;
    float m_speed = 300.0f;
    int m_damage = 500;

    float m_penetrationChance;   // 0~1，当前子弹的穿透概率
    int   m_remainingPenetrations; // 剩余可穿透次数，默认为0（无穿透）
};
