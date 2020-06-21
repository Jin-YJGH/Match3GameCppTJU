#include "GameEndScene.h"
#include "HelloWorldScene.h"
#include "StartScene.h"
#include "LevelChooseScene.h"

GameEnd::GameEnd()
{
	_endType = "NULL";
	_points = 0;
	_level = 1;
}

/*
Scene* GameEnd::createScene()
{
	return GameEnd::create();
}
*/

GameEnd* GameEnd::create(std::string& endType, int& points, int& level)
{
	auto gameEnd = new GameEnd;
	if (gameEnd && gameEnd->init(endType, points, level)) {
		gameEnd->autorelease();
		return gameEnd;
	}
	CC_SAFE_DELETE(gameEnd);
	return nullptr;
}

bool GameEnd::init(std::string& endType, int& points, int& level)
{
	if (!Scene::init()) {
		return false;
	}

	auto size = Director::getInstance()->getVisibleSize();
	auto origin = Director::getInstance()->getVisibleOrigin();

	auto bkg = Sprite::create("bkg.png");
	auto x = origin.x + size.width / 2;
	auto y = origin.y + size.height / 2;
	bkg->setPosition(Vec2(x, y));
	addChild(bkg, -1);

	_level = level;

	TTFConfig labelConfig;
	labelConfig.fontFilePath = "arial.ttf";
	labelConfig.fontSize = 48;
	labelConfig.bold = true;

	if (endType == "GAME OVER") {
		auto titleLabel = Label::createWithTTF(labelConfig, "GAME OVER");
		titleLabel->setTextColor(Color4B::RED);
		auto location = Vec2(origin.x + size.width / 2, origin.y + size.height * 0.90);
		titleLabel->setPosition(location);
		this->addChild(titleLabel);
		auto pointsLabel = Label::createWithTTF(labelConfig, "Points: " + std::to_string(points));
		pointsLabel->setTextColor(Color4B::BLACK);
		pointsLabel->setPosition(origin.x + size.width / 2, origin.y + size.height * 0.80);
		this->addChild(pointsLabel);
	}
	else if (endType == "YOU WIN") {
		auto titleLabel = Label::createWithTTF(labelConfig, "YOU WIN");
		titleLabel->setTextColor(Color4B::GREEN);
		auto location = Vec2(origin.x + size.width / 2, origin.y + size.height * 0.90);
		titleLabel->setPosition(location);
		this->addChild(titleLabel);
		auto pointsLabel = Label::createWithTTF(labelConfig, "Points: " + std::to_string(points));
		pointsLabel->setTextColor(Color4B::BLACK);
		pointsLabel->setPosition(origin.x + size.width / 2, origin.y + size.height * 0.80);
		this->addChild(pointsLabel);
	}
	else {
		return false;
	}

	Vector<MenuItem*> exitChoices;
	
	MenuItemImage* exitToStart = MenuItemImage::create("EXIT_TO_START_MENU1.png", "EXIT_TO_START_MENU2.png",
														CC_CALLBACK_1(GameEnd::menuExitToStartCallback, this));
	MenuItemImage* exitToLevelChoose = MenuItemImage::create("EXIT_TO_LEVEL_CHOOSE1.png", "EXIT_TO_LEVEL_CHOOSE2.png",
															  CC_CALLBACK_1(GameEnd::menuExitToLevelChooseCallback, this));
	MenuItemImage* exitToWindows = MenuItemImage::create("EXIT_TO_WINDOWS1.png", "EXIT_TO_WINDOWS2.png",
														  CC_CALLBACK_1(GameEnd::menuExitToWindowsCallback, this));
	MenuItemImage* playAgain = MenuItemImage::create("PLAY_AGAIN1.png", "PLAY_AGAIN2.png",
													  CC_CALLBACK_1(GameEnd::menuPlayAgainCallback, this));

	x = origin.x + size.width / 2;
	y = origin.y + size.height * 0.65;
	playAgain->setPosition(Vec2(x, y));
	exitChoices.pushBack(playAgain);
	y = origin.y + size.height * 0.50;
	exitToStart->setPosition(Vec2(x, y));
	exitChoices.pushBack(exitToStart);
	y = origin.y + size.height * 0.35;
	exitToLevelChoose->setPosition(Vec2(x, y));
	exitChoices.pushBack(exitToLevelChoose);
	y = origin.y + size.height * 0.20;
	exitToWindows->setPosition(Vec2(x, y));
	exitChoices.pushBack(exitToWindows);

	auto exitMenu = Menu::createWithArray(exitChoices);
	exitMenu->setPosition(Vec2::ZERO);
	this->addChild(exitMenu);

	return true;
}

void GameEnd::menuExitToStartCallback(cocos2d::Ref* pSender)
{
	auto nextScene = Start::createScene();
	Director::getInstance()->replaceScene(
		TransitionSlideInT::create(1.0f / 60, nextScene));
	MenuItem* item = (MenuItem*)pSender;
}

void GameEnd::menuExitToLevelChooseCallback(cocos2d::Ref* pSender)
{
	auto nextScene = LevelChoose::createScene();
	Director::getInstance()->replaceScene(
		TransitionSlideInT::create(1.0f / 60, nextScene));
	MenuItem* item = (MenuItem*)pSender;
}

void GameEnd::menuExitToWindowsCallback(cocos2d::Ref* pSender)
{
	Director::getInstance()->end();
}

void GameEnd::menuPlayAgainCallback(cocos2d::Ref* pSender)
{
	auto nextScene = HelloWorld::create(_level);
	Director::getInstance()->replaceScene(
		TransitionSlideInT::create(1.0f / 60, nextScene));
	MenuItem* item = (MenuItem*)pSender;
}