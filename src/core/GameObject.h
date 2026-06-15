//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:
//create time : 2026-06-15 11:45:25

#pragma once
#include <QObject>
#include <QRectF>

class GameObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect NOTIFY rectChanged)
protected:
    bool m_markedForDelete = false;
public:
    bool isMarkedForDelete() const { return m_markedForDelete; }
    void markForDelete() { m_markedForDelete = true; }
public:
    explicit GameObject(QObject *parent = nullptr);
    virtual ~GameObject();
    virtual QRectF rect() const = 0;
    virtual void update(int deltaMs) = 0;
    virtual int type() const = 0;        // 0玩家,1敌人,2子弹等
    virtual void onCollision(GameObject* other) {}

signals:
    void rectChanged();
};
