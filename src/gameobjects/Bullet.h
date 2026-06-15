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

private:
    QRectF m_rect;
    QPointF m_velocity;
    float m_speed = 10.0f;
    int m_damage = 1000;
};
