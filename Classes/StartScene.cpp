#include "StartScene.h"
#include "HelloWorldScene.h"
#include "LevelChooseScene.h"
#include "SettingScene.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;

Scene* Start::createScene()
{
    return Start::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in StartScene.cpp\n");
}

bool Start::init()
{
    if (!Scene::init())
    {
        return false;
    }
    SimpleAudioEngine::getInstance()->preloadBackgroundMusic(
        FileUtils::getInstance()->fullPathForFilename("bgm.mp3").c_str());
    SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(0.5f);
    SimpleAudioEngine::getInstance()->playBackgroundMusic("bgm.mp3", true);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bkg = cocos2d::LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(bkg);

    MenuItemImage* startMenu = MenuItemImage::create(
        "START1.png", "START2.png",
        CC_CALLBACK_1(Start::menuStartCallback, this));
    //
    MenuItemImage* settingMenu = MenuItemImage::create(
        "SETTING1.png", "SETTING2.png",
        CC_CALLBACK_1(Start::menuSettingCallback, this));
    //
    if (startMenu == nullptr ||
        startMenu->getContentSize().width <= 0 ||
        startMenu->getContentSize().height <= 0)
    {
        problemLoading("START.png");
    }
    else
    {
        float x = origin.x + visibleSize.width / 2;
        float y = origin.y + visibleSize.height / 2 + startMenu->getContentSize().height +startMenu->getContentSize().height;
        startMenu->setPosition(Vec2(x, y));
    }
    //
    if (settingMenu == nullptr ||
        settingMenu->getContentSize().width <= 0 ||
        settingMenu->getContentSize().height <= 0)
    {
        problemLoading("SETTING.png");
    }
    else
    {
        float x = origin.x + visibleSize.width / 2;
        float y =2* origin.y + visibleSize.height / 2 + startMenu->getContentSize().height;
        settingMenu->setPosition(Vec2(x, y));
    }
    //
    auto menu = Menu::create(startMenu, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    //
    auto menu2 = Menu::create(settingMenu, NULL);
    menu2->setPosition(Vec2::ZERO);
    this->addChild(menu2, 1);
    //
    return true;
}

void Start::menuStartCallback(cocos2d::Ref* pSender)
{
    auto nextScene = LevelChoose::createScene();
    Director::getInstance()->replaceScene(
        TransitionSlideInT::create(1.0f / 60, nextScene));
    MenuItem* item = (MenuItem*)pSender;
}
//
void Start::menuSettingCallback(cocos2d::Ref* pSender)
{
    auto nextScene2 = Setting::createScene();
    Director::getInstance()->replaceScene(
        TransitionSlideInT::create(1.0f / 60, nextScene2));
    MenuItem* item = (MenuItem*)pSender;
}
//