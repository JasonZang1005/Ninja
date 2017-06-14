#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "GameScene.h"
#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

using namespace  rapidjson;

Scene* LoginScene::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = LoginScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}





// on "init" you need to initialize your instance
bool LoginScene::init()
{
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size size = Director::getInstance()->getVisibleSize();
	visibleHeight = size.height;
	visibleWidth = size.width;

	auto button = Button::create();
	button->setTitleText("Start Game");
	button->setTitleFontSize(30);
	button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	this->addChild(button, 2);

	button->addClickEventListener(CC_CALLBACK_1(LoginScene::menuChangeitem, this));


	return true;
}

void LoginScene::menuChangeitem(Ref*pSender) {

	auto sence = GameScene::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, sence, Color3B(0, 255, 255)));
}



