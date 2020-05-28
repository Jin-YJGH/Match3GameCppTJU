#include "StartScene.h"
#include "HelloWorldScene.h"

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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto bkg = cocos2d::LayerColor::create(Color4B(255, 255, 255, 255));
    this->addChild(bkg);

    MenuItemImage* startMenu = MenuItemImage::create(
        "START1.png", "START2.png",
        CC_CALLBACK_1(Start::menuStartCallback, this));

    if (startMenu == nullptr ||
        startMenu->getContentSize().width <= 0 ||
        startMenu->getContentSize().height <= 0)
    {
        problemLoading("START.png");
    }
    else
    {
        float x = origin.x + visibleSize.width / 2;
        float y = origin.y + visibleSize.height / 2 + startMenu->getContentSize().height;
        startMenu->setPosition(Vec2(x, y));
    }

    auto menu = Menu::create(startMenu, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    return true;
}

void Start::menuStartCallback(cocos2d::Ref* pSender)
{
    auto nextScene = HelloWorld::createScene();
    Director::getInstance()->replaceScene(
        TransitionSlideInT::create(1.0f / 60, nextScene));
    MenuItem* item = (MenuItem*)pSender;
}