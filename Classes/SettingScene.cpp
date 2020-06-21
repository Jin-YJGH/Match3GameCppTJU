#include "SettingScene.h"
#include "HelloWorldScene.h"
#include "StartScene.h"

using namespace cocos2d::ui;
using namespace CocosDenshion;

Scene* Setting::createScene()
{
	auto scene = Scene::create();
	auto layer = Setting::create();
	scene->addChild(layer);
	return Setting::create();
}

bool Setting::init()
{
	if (!Scene::init()) {
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bkg = cocos2d::LayerColor::create(Color4B::WHITE);
	this->addChild(bkg);

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

	//auto sprite = Sprite::create("START.png");
	//sprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//sprite->setScale(2.0f, 1.1788f);
	//this->addChild(sprite);
	//auto back2 = MenuItemImage::create("BACK.png", "BACK.png", CC_CALLBACK_0(Setting::menuCallBack, this));
	//back2->setPosition(Vec2(900, 50));
	//back2->setScale(2.0f);
	//auto menu = Menu::create(back2, NULL);
	//menu->setPosition(Vec2::ZERO);
	//this->addChild(menu, 2);
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic(
		FileUtils::getInstance()->fullPathForFilename("bgm.mp3").c_str());
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f);
	SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);
	createSlider();

	return true;
}

void Setting::createSlider() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto sl = Slider::create();
	sl->loadBarTexture("SLIDER_TRACK.png");
	sl->loadSlidBallTextures("BUTTON1.png", "BUTTON2.png", "BUTTON1.png");
	sl->loadProgressBarTexture("SLIDER_TRACK.png");
	sl->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	sl->setPercent(75);
	sl->addEventListener(CC_CALLBACK_2(Setting::SliderCallBack, this));
	this->addChild(sl);
}

void Setting::SliderCallBack(Ref* pSender, Slider::EventType type) {
	auto item = (Slider*)(pSender);
	log("%i", item->getPercent());
	SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(item->getPercent() / 100.0f);
}

void Setting::menuCallBack() {
	auto scene = Start::createScene();
	auto transition = TransitionSlideInL::create(1.0, scene);
	Director::getInstance()->replaceScene(transition);
}

