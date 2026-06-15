#pragma once
#include "../core/GameObject.h"

class ExpOrb : public GameObject {
    Q_OBJECT
public:
    ExpOrb(const QPointF& pos, int value, QObject* parent = nullptr);
    QRectF rect() const override { return m_rect; }
    int type() const override { return 3; } // 类型3：经验球
    void update(int deltaMs) override;
    void setTarget(const QPointF& target);
    int value() const { return m_value; }
    bool isMovingToPlayer() const { return m_movingToPlayer; }

private:
    QRectF m_rect;
    int m_value;
    QPointF m_target;
    bool m_movingToPlayer = false;
    float m_speed = 300.0f; // 飞行速度（像素/秒）
};
