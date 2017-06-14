#include "GameScene.h"
#include "cocos2d.h"
#include <algorithm>
#include "SimpleAudioEngine.h"
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameScene::init() {
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	visibleHeight = visibleSize.height;
	visibleWidth = visibleSize.width;
	isJumping = false;
//背景：
	sky = Sprite::create("sky.png");
	sky->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2+sky->getContentSize().height/2));
	sky->setScale(2.4);
	this->addChild(sky, 0);

	scenery = Sprite::create("scenery.png");
	scenery->setPosition(Vec2(visibleSize.width / 2, scenery->getContentSize().height / 2));
	this->addChild(scenery, 0);

	treeRootLeft = Sprite::create("tree-root-left.png");
	treeRootLeft->setPosition(Vec2(treeRootLeft->getContentSize().width / 2, treeRootLeft->getContentSize().height / 2));
	this->addChild(treeRootLeft, 1);

    treeRootRight = Sprite::create("tree-root-right.png");
	treeRootRight->setPosition(Vec2(visibleSize.width - treeRootRight->getContentSize().width / 2, treeRootRight->getContentSize().height / 2));
	this->addChild(treeRootRight, 1);

	treeLeft = Sprite::create("tree.png");
	treeLeft->setPosition(Vec2(0, treeRootLeft->getContentSize().height + treeLeft->getContentSize().height / 2));
	treeLeft->setScaleY(visibleHeight / treeLeft->getContentSize().height);
	this->addChild(treeLeft, 1);

	treeLeft1 = Sprite::create("tree.png");
	treeLeft1->setPosition(Vec2(0, treeRootLeft->getContentSize().height + treeLeft1->getContentSize().height / 2));
	treeLeft1->setScaleY( visibleHeight / treeLeft1->getContentSize().height);
	this->addChild(treeLeft1, 1);

	treeRight = Sprite::create("tree.png");
	treeRight->setScaleY(visibleHeight / treeRight->getContentSize().height);
	treeRight->setPosition(Vec2(visibleSize.width, treeRootRight->getContentSize().height + treeRight->getContentSize().height / 2));
	this->addChild(treeRight, 1);

	treeRight1 = Sprite::create("tree.png");
	treeRight1->setScaleY(visibleHeight / treeRight1->getContentSize().height);
	treeRight1->setPosition(Vec2(visibleSize.width, treeRootRight->getContentSize().height + treeRight1->getContentSize().height / 2));
	this->addChild(treeRight1, 1);
	

//人物：

	player = Sprite::createWithSpriteFrameName("ninja-run-0.png");
	runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
	player->runAction(RepeatForever::create(runAnimate));
	runAnimate->setTag(0);
	


	player->setPosition(Vec2(treeRootLeft->getContentSize().width / 2 - 10 + player->getContentSize().width / 2, visibleSize.height / 4));
	this->addChild(player, 1, "ninja");
	side = 'l';

//人物跑动的距离得分：
	score = 0;
	std::string temp = cocos2d::Value(score).asString();
	score_ = Label::create(temp, "fonts/arial.ttf", 36);
	score_->setPosition(Vec2(visibleWidth / 2, visibleHeight - 30));
	this->addChild(score_);

	schedule(schedule_selector(GameScene::BackGround));//背景滚动
	addTouchListener();//鼠标点击事件
	schedule(schedule_selector(GameScene::createObstacle), 4.0f, kRepeatForever, 0);//添加障碍物
	schedule(schedule_selector(GameScene::collision), 0.5f, kRepeatForever, 0);//判断碰撞
	obstacleList = Array::create();
	obstacleList->retain();
	
	return true;
}
//背景滚动
void GameScene::BackGround(float dt) {
	score += 5;
	std::string temp = cocos2d::Value(score).asString();
	score_->setString(temp);
	treeRootLeft->setPositionY(treeRootLeft->getPositionY() - 8);
	treeRootRight->setPositionY(treeRootRight->getPositionY() - 8);
	scenery->setPositionY(scenery->getPositionY() - 8);
	treeLeft->setPositionY(treeLeft->getPositionY() - 8);
	treeLeft1->setPositionY(treeLeft1->getPositionY() - 8);
	treeRight->setPositionY(treeRight->getPositionY() - 8);
	treeRight1->setPositionY(treeRight1->getPositionY() - 8);

	if (treeLeft->getPositionY() <= -visibleHeight / 2) {
		treeLeft->setPositionY(visibleHeight*3/2);
		treeLeft1->setPositionY(visibleHeight / 2);
		treeRight->setPositionY(visibleHeight * 3 / 2);
		treeRight1->setPositionY(visibleHeight / 2);
	}
	if (treeLeft1->getPositionY() <= -visibleHeight / 2) {
		treeLeft1->setPositionY(visibleHeight*3/2);
		treeLeft->setPositionY(visibleHeight / 2);
		treeRight1->setPositionY(visibleHeight * 3 / 2);
		treeRight->setPositionY(visibleHeight / 2);
	}

	//障碍物滚动
	for (int i = obstacleList->count() - 1; i >= 0; i--)
	{
		auto s = (Sprite*)obstacleList->getObjectAtIndex(i);
		s->setPositionY(s->getPositionY() - 8);
		if (s->getPositionY() < -s->getContentSize().height / 2)
		{
			obstacleList->removeObjectAtIndex(i);
			this->removeChild(s);
			cocos2d::log("a obstacle removed");
		}
	}
}

//添加障碍
void GameScene::createObstacle(float dt) {
	Sprite* left = Sprite::create("obstacle_l.png");
	Sprite* right = Sprite::create("obstacle_r.png");
	obstacleList->addObject(left);
	obstacleList->addObject(right);
	left->setPosition(5, visibleHeight - 20);
	right->setPosition(visibleWidth - 5, visibleHeight - 400);
	this->addChild(left,1);
	this->addChild(right,1);
}
//判断碰撞
void GameScene::collision(float dt) {
	Rect temp = player->getBoundingBox();
	for (int i = 0; i < obstacleList->count(); i++)
	{
		Sprite* obstacleSprite = (Sprite*)obstacleList->getObjectAtIndex(i);
		bool pia = temp.intersectsRect(obstacleSprite->getBoundingBox());
		if (pia == true)
		{
			unscheduleAllSelectors();
			break;
		}
	}
}

//鼠标点击事件
void GameScene::addTouchListener() {
	// Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, player);
}


bool GameScene::onTouchBegan(Touch *touch, Event *event) {
	if (isJumping) {
		return false;
	}
	isJumping = true;
	player->stopAllActions();
	if (side == 'l') {
		auto x = visibleWidth - player->getContentSize().width;
		auto jumpAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaJumpAnimation"));
		auto spawn = Spawn::createWithTwoActions(MoveTo::create(1.2f, Vec2(x, player->getPositionY())), jumpAnimate);
		bool& jumping = isJumping;
		auto seq = Sequence::create(spawn, CallFunc::create([&]() {
			player->setFlipX(true);
			auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
			player->runAction(RepeatForever::create(runAnimate));
			jumping = false;
		}),nullptr);
		player->runAction(seq);
		side = 'r';
	}
	else if (side == 'r') {
		auto x = player->getContentSize().width + 2;
		auto jumpAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaJumpAnimation"));
		auto spawn = Spawn::createWithTwoActions(MoveTo::create(1.2f, Vec2(x, player->getPositionY())), jumpAnimate);
		bool& jumping = isJumping;
		auto seq = Sequence::create(spawn, CallFunc::create([&]() {
			player->setFlipX(false);
			auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
			player->runAction(RepeatForever::create(runAnimate));
			jumping = false;
		}),nullptr);
		player->runAction(seq);
		side = 'l';
	}
	
	return true;
}

void GameScene::onTouchEnded(Touch *touch, Event *event) {


}

