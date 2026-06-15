//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:
//create time : 2026-06-15 11:55:33

#pragma once
#include "Enemy.h"
class PlayScene; // 前向声明
class PaimonEnemy : public Enemy {
    Q_OBJECT
public:
   explicit PaimonEnemy(const QRectF& startRect, PlayScene* scene, QObject *parent = nullptr);
    void update(int deltaMs) override;
private:
    QObject* m_player;  // 指向玩家对象（PlayScene 或 Player）
      PlayScene* m_scene;
};
