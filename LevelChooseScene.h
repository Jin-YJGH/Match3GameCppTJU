#pragma once
#ifndef __LEVEL_CHOOSE_SCENE_H__
#define __LEVEL_CHOOSE_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class LevelChoose : public Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	//void menuLevelCallback(cocos2d::Ref* pSender);
	void menuBackCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(LevelChoose);
};
#endif // !__LEVEL_CHOOSE_SCENE_H__
