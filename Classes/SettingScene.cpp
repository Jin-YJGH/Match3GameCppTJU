#include "SettingScene.h"
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h" 
#include"GUI/CCControlExtension/CCInvocation.h"

using namespace ui;
using namespace CocosDenshion;

auto volume = Slider::create();
volume->loadBarTexture("sliderback.png");//音量进度条背景
volume->loadSlidBallTextures("sliderbutton.png", "sliderbutton.png", "");//滑动块（正常，按下，禁用
volume->loadProgressBarTexture("silderback.png");//滑动条进度图片，即从左至滑动按钮显示的内容
volume->setPercent(soundValue * 100);
volume->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
volume->addEventListener([=](Ref* pSender, Slider::EventType type) 
	{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) 
	{
		soundValue = volume->getPercent() * 0.01;
		log("NOW=%.2f", soundValue);
		// 设置背景音乐值
		SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(soundValue);
		// 设置音效值
		//SimpleAudioEngine::getInstance()->setEffectsVolume(soundValue);
		UserDefault::getInstance()->setFloatForKey("soundNum", soundValue);
		UserDefault::getInstance()->flush();
	}
	});
this->addChild(volume);
//声音开关
auto soundControl = ControlSwitch::create
(
	Sprite::create("switch-mask.png"),
	Sprite::create("switch-on.png"),
	Sprite::create("switch-off.png"),
	Sprite::create("switch-thumb.png"),
	Label::createWithSystemFont("On", "Arial-BoldMT", 16),
	Label::createWithSystemFont("Off", "Arial-BoldMT", 16)
);
soundControl->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::soundSwitch), Control::EventType::VALUE_CHANGED);
soundControl->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.3));
this->addChild(soundControl);
}

void HelloWorld::soundSwitch(Ref* psender, Control::EventType controlevent) {
	auto fSwitch = (ControlSwitch*)psender;
	if (fSwitch->isOn()) {
		//SimpleAudioEngine::getInstance()->resumeAllEffects();
		SimpleAudioEngine::getInstance()->rewindBackgroundMusic();
	}
	else {
		//SimpleAudioEngine::getInstance()->stopAllEffects();
		SimpleAudioEngine::getInstance()->stopBackgroundMusic();
	}
}
