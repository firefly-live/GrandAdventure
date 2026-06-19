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


    //敌人死亡状态

    //关于击退
    QPointF m_knockbackVelocity;   // 击退速度（像素/帧）
    float m_knockbackDecay = 0.8f; // 每帧衰减系数（0.8~0.9），使击退快速停止


    //关于闪白
    bool m_whiteFlash = false;
    int m_flashTimer = 0;

public:
    void startDeath(int durationMs = 500);
    bool isDying() const { return m_isDying; }
    bool isReadyToDelete() const { return m_readyToDelete; }
    bool isAlive() const { return !m_isDying && !m_readyToDelete; }


    //关于击退

    void applyKnockback(const QPointF& direction, float force);
    bool hasKnockback() const { return m_knockbackVelocity.manhattanLength() > 0.1f; }


    //关于闪白
    void triggerFlash(int durationMs = 80);
    bool isFlashing() const { return m_whiteFlash; }
    void updateFlash(int deltaMs);



    //-----------------关于youhua

    int maxHp = 20;
    int damage = 5;

    void setMaxHp(int hp) { maxHp = hp; this->hp = hp; }
    void setSpeed(float sp) { speed = sp; }
    void setDamage(int dmg) { damage = dmg; }
    int getDamage() const { return damage; }
    int getMaxHp() const { return maxHp; }


protected:
    void updateDeath(int deltaMs); // 在子类 update 中调用
    bool m_isDying = false;
    bool m_readyToDelete = false;
    int m_deathTimer = 0;






};
