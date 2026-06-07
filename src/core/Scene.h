//create by :唐晟庆2024051604041
//email: 2052448030@qq.com
//models: 作为场景基类,用作场景得设置,然后方便子类继承该场景
//create time : 2026-06-07 11:33:50
#pragma once
#include<QObject>
class Scene: public QObject{
    Q_OBJECT
public:
    explicit Scene(QObject *parent  = nullptr){};
    virtual ~Scene(){};

    virtual void onEnter(){};//调用
    virtual void onExit(){};//退出
    virtual void update(int deltaMs)=0;//更新逻辑
    virtual void draw()=0;



};
