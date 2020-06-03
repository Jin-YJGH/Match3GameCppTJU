#pragma once
#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
#include"GUI\CCControlExtension\CCControl.h"
#include"GUI\CCControlExtension\CCControlSwitch.h"
using namespace cocos2d::extension;

USING_NS_CC;
float soundValue;
void soundSwitch(Ref* psender, Control::EventType controlevent);

#endif // !__SETTING_SCENE_H__
