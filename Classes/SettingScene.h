#pragma once
#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "StartScene.h"
#include "cocos2d.h"
#include"GUI\CCControlExtension\CCControl.h"
#include"GUI\CCControlExtension\CCControlSwitch.h"
#include "ui/CocosGUI.h" 
#include"GUI/CCControlExtension/CCInvocation.h"
#include "cocos2d\cocos\audio\include\SimpleAudioEngine.h" 

using namespace cocos2d::extension;
using namespace cocos2d::ui;
using namespace CocosDenshion;

USING_NS_CC;
void soundSwitch(Ref* psender, Control::EventType controlevent);


class Setting :public Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    void menuCallBack();
    void createSlider();
    void SliderCallBack(Ref* pSender, Slider::EventType type);
    CREATE_FUNC(Setting);

private:
    float soundValue;
};

#endif // !__SETTING_SCENE_H__
