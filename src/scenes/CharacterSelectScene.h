//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models:   选择角色界面,现在用于测试
//create time : 2026-06-07 14:31:21

#pragma once
#include "../core/Scene.h"

class CharacterSelectScene : public Scene {
    Q_OBJECT
public:
    void update(int deltaMs) override;
    void draw() override;
};
