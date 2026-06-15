//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:
//create time : 2026-06-15 11:46:06

#pragma once
#include "../core/GameObject.h"

class Enemy : public GameObject {
    Q_OBJECT
public:
    explicit Enemy(QObject *parent = nullptr);
    QRectF rect() const override { return m_rect; }
    int type() const override { return 1; }
    void setRect(const QRectF& r);

    // 敌人公共属性
    float speed = 1.0f;
    int direction = 2;   // 1左2右
    int frameIndex = 0;


    // 反馈交互
    int hp = 1000;
    void takeDamage(int damage);
protected:
    QRectF m_rect;
};
