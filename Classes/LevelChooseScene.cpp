#include "LevelChooseScene.h"
#include "HelloWorldScene.h"
#include "StartScene.h"

#define TOTAL_LEVELS (2)

Scene* LevelChoose::createScene()
{
	return LevelChoose::create();
}

bool LevelChoose::init()
{
	if (!Scene::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bkg = cocos2d::LayerColor::create(Color4B::WHITE);
	this->addChild(bkg);

	Vector<MenuItem*> levelChoices;
	for (int i = 1; i <= TOTAL_LEVELS; i++) {
		std::string img1 = "LEVEL" + std::to_string(i) + "_1.png";
		std::string img2 = "LEVEL" + std::to_string(i) + "_2.png";
		MenuItemImage* levelChoice = MenuItemImage::create(img1, img2, 
			[&](Ref* pSender) {
				MenuItem* item = (MenuItem*)pSender;
				int level = (item->getName() == "LEVEL1") ? 1 : 2;
				auto nextScene = HelloWorld::create(level);
				Director::getInstance()->replaceScene(
					TransitionSlideInT::create(1.0f / 60, nextScene));
			});
		levelChoice->setName("LEVEL" + std::to_string(i));
		auto x = origin.x + visibleSize.width / 2;
		auto y = origin.y + (visibleSize.height - (TOTAL_LEVELS - 1) * levelChoice->getContentSize().height) / 2 + 
							(TOTAL_LEVELS - i) * levelChoice->getContentSize().height;
		levelChoice->setPosition(Vec2(x, y));
		levelChoices.pushBack(levelChoice);
	}
	auto levelMenu = Menu::createWithArray(levelChoices);
	levelMenu->setPosition(Vec2::ZERO);
	this->addChild(levelMenu);

	MenuItemImage* back = MenuItemImage::create("BACK1.png", "BACK2.png",
		[&](Ref* pSender) {
			auto nextScene = Start::createScene();
			Director::getInstance()->replaceScene(
				TransitionSlideInT::create(1.0f / 60, nextScene));
			MenuItem* item = (MenuItem*)pSender;
		});
	auto x = origin.x + visibleSize.width * 0.10;
	auto y = origin.y + visibleSize.height * 0.90;
	back->setPosition(Vec2(x, y));
	auto backMenu = Menu::create(back, NULL);
	backMenu->setPosition(Vec2::ZERO);
	this->addChild(backMenu);

	return true;
}

/*
void LevelChoose::menuLevelCallback(cocos2d::Ref* pSender)
{
	MenuItem* item = (MenuItem*)pSender;
	int level = (item->getName() == "LEVEL1") ? 1 : 2;
	auto nextScene = HelloWorld::create(level);
	Director::getInstance()->replaceScene(
		ransitionSlideInT::create(1.0f / 60, nextScene));
}
*/