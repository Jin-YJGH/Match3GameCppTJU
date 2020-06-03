#include "SettingScene.h"
#include "HelloWorldScene.h"
#include "ui/CocosGUI.h" 
#include"GUI/CCControlExtension/CCInvocation.h"

using namespace ui;
using namespace CocosDenshion;

auto volume = Slider::create();
volume->loadBarTexture("sliderback.png");//��������������
volume->loadSlidBallTextures("sliderbutton.png", "sliderbutton.png", "");//�����飨���������£�����
volume->loadProgressBarTexture("silderback.png");//����������ͼƬ����������������ť��ʾ������
volume->setPercent(soundValue * 100);
volume->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
volume->addEventListener([=](Ref* pSender, Slider::EventType type) 
	{
	if (type == Slider::EventType::ON_PERCENTAGE_CHANGED) 
	{
		soundValue = volume->getPercent() * 0.01;
		log("NOW=%.2f", soundValue);
		// ���ñ�������ֵ
		SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(soundValue);
		// ������Чֵ
		//SimpleAudioEngine::getInstance()->setEffectsVolume(soundValue);
		UserDefault::getInstance()->setFloatForKey("soundNum", soundValue);
		UserDefault::getInstance()->flush();
	}
	});
this->addChild(volume);
//��������
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
