#pragma once
#ifndef __GAME_END_SCENE__
#define __GAME_END_SCENE__

#include "cocos2d.h"

USING_NS_CC;

class GameEnd : public cocos2d::Scene
{
public:
	GameEnd();
	//static cocos2d::Scene* createScene();

	virtual bool init(std::string& endType, int& points, int& level);
	static GameEnd* create(std::string& endType, int& points, int& level);

	CC_SYNTHESIZE(std::string, _endType, EndType);
	CC_SYNTHESIZE(int, _points, Points);

	void menuExitToStartCallback(cocos2d::Ref* pSender);
	void menuExitToLevelChooseCallback(cocos2d::Ref* pSender);
	void menuExitToWindowsCallback(cocos2d::Ref* pSender);
	void menuPlayAgainCallback(cocos2d::Ref* pSender);
private:
	int _level;
};

#endif // !__GAME_END_SCENE__
