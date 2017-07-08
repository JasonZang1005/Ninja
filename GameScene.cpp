#include "GameScene.h"
#include "cocos2d.h"
#include <algorithm>
#include "SimpleAudioEngine.h"
#include "cocostudio/CocoStudio.h"
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

	isShield = false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	visibleHeight = visibleSize.height;
	visibleWidth = visibleSize.width;
	isJumping = false;
	//背景：
	sky = Sprite::create("sky.png");
	sky->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + sky->getContentSize().height / 2));
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
	//treeLeft->setScaleY(visibleHeight / treeLeft->getContentSize().height);
	this->addChild(treeLeft, 1);

	treeLeft1 = Sprite::create("tree.png");
	treeLeft1->setPosition(Vec2(0, treeRootLeft->getContentSize().height + treeLeft1->getContentSize().height / 2 + treeLeft->getContentSize().height));
	//treeLeft1->setScaleY( visibleHeight / treeLeft1->getContentSize().height);
	this->addChild(treeLeft1, 1);

	treeRight = Sprite::create("tree.png");
	//treeRight->setScaleY(visibleHeight / treeRight->getContentSize().height);
	treeRight->setPosition(Vec2(visibleSize.width, treeRootRight->getContentSize().height + treeRight->getContentSize().height / 2));
	this->addChild(treeRight, 1);

	treeRight1 = Sprite::create("tree.png");
	//treeRight1->setScaleY(visibleHeight / treeRight1->getContentSize().height);
	treeRight1->setPosition(Vec2(visibleSize.width, treeRootRight->getContentSize().height + treeRight1->getContentSize().height / 2 + treeRight->getContentSize().height));
	this->addChild(treeRight1, 1);

	/* monster dead animation */
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}


	//人物：

	player = Sprite::createWithSpriteFrameName("ninja-run-0.png");
	runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
	player->runAction(RepeatForever::create(runAnimate));
	player->setAnchorPoint(Vec2(0.5, 0.5));
	runAnimate->setTag(0);

	player->setPosition(Vec2(player->getContentSize().width + 2, visibleSize.height / 4));
	this->addChild(player, 2);
	side = 'l';

	//人物跑动的距离得分：
	height = 0;
	std::string temp = cocos2d::Value(height).asString();
	height_ = Label::create(temp, "fonts/arial.ttf", 36);
	height_->setPosition(Vec2(visibleWidth / 2, visibleHeight - 30));
	this->addChild(height_);
	//击杀怪物得分
	
	score = 0;
	std::string temp1 = "score: "+ cocos2d::Value(score).asString();
	score_ = Label::create(temp1, "fonts/arial.ttf", 36);
	score_->setPosition(Vec2(120, 30));
	this->addChild(score_);

	//Start Game
	button = Button::create();
	button->setTitleText("Start Game");
	button->setTitleFontSize(30);
	button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	this->addChild(button, 2);

	button->addClickEventListener(CC_CALLBACK_1(GameScene::menuChangeitem, this));

	schedule(schedule_selector(GameScene::obstacleCollision), 0.5f, kRepeatForever, 0);//判断障碍物碰撞
	schedule(schedule_selector(GameScene::shieldCollision), 0.5f, kRepeatForever, 0);//判断盾牌碰撞
	obstacleList = Array::create();
	obstacleList->retain();
	shieldList = Array::create();
	shieldList->retain();
	monsterList = Array::create();
	monsterList->retain();
	ropeList = Array::create();
	ropeList->retain();

	return true;
}

//Start Game
void GameScene::menuChangeitem(Ref*pSender) {
	addTouchListener();//鼠标点击事件
					   //schedule(schedule_selector(GameScene::createObstacle), 4.0f, kRepeatForever, 0);//障碍
	schedule(schedule_selector(GameScene::createShield), 6.0f, kRepeatForever, 0);//保护罩
	schedule(schedule_selector(GameScene::createMonsterAndRope), 6.0f, kRepeatForever, 0);//怪物与绳子
	schedule(schedule_selector(GameScene::BackGround));//背景滚动
	schedule(schedule_selector(GameScene::killmonster));//杀灭怪物
	schedule(schedule_selector(GameScene::randomcreate));
	button->setVisible(false);
}
//背景滚动
void GameScene::backgroundPositionSub(Sprite* x,int dis) {
	x->setPositionY(x->getPositionY()-dis);
}

void GameScene::BackGround(float dt) {
	int dis;
	if (fabs(dt - 0.008) <=0.004) {
		dis = 20;
		height += 4;
	}
	else {
		dis = dt * 500;
		height += 2;
	}
	
	std::string temp = cocos2d::Value(height).asString();
	height_->setString(temp);
	backgroundPositionSub(treeRootRight,dis);
	backgroundPositionSub(treeRootLeft, dis);
	backgroundPositionSub(scenery, dis);
	backgroundPositionSub(treeLeft, dis);
	backgroundPositionSub(treeLeft1, dis);
	backgroundPositionSub(treeRight, dis);
	backgroundPositionSub(treeRight1, dis);
	if (treeLeft->getPositionY() <= -visibleHeight / 2) {
		treeLeft->setPositionY(visibleHeight * 3 / 2);
		treeLeft1->setPositionY(visibleHeight / 2);
		treeRight->setPositionY(visibleHeight * 3 / 2);
		treeRight1->setPositionY(visibleHeight / 2);
	}
	if (treeLeft1->getPositionY() <= -visibleHeight / 2) {
		treeLeft1->setPositionY(visibleHeight * 3 / 2);
		treeLeft->setPositionY(visibleHeight / 2);
		treeRight1->setPositionY(visibleHeight * 3 / 2);
		treeRight->setPositionY(visibleHeight / 2);
	}

	//障碍物滚动
	for (int i = obstacleList->count() - 1; i >= 0; i--)
	{
		auto s = (Sprite*)obstacleList->getObjectAtIndex(i);
		backgroundPositionSub(s, dis);
		if (s->getPositionY() < -s->getContentSize().height / 2)
		{
			obstacleList->removeObjectAtIndex(i);
			this->removeChild(s);
			cocos2d::log("a obstacle removed");
		}
	}
	//防护罩滚动
	for (int i = shieldList->count() - 1; i >= 0; i--)
	{
		auto s = (Sprite*)shieldList->getObjectAtIndex(i);
		backgroundPositionSub(s, dis);
		if (s->getPositionY() < -s->getContentSize().height / 2)
		{
			shieldList->removeObjectAtIndex(i);
			this->removeChild(s);
			cocos2d::log("a shield removed");
		}
	}
	for (int i = monsterList->count() - 1; i >= 0; i--)
	{
		auto s = (Sprite*)monsterList->getObjectAtIndex(i);
		backgroundPositionSub(s, dis);
		s->setPositionX(s->getPositionX() - 4);
		if (!killone) {
			killone = true;
			continue;
		}
		if (s->getPositionY() < -s->getContentSize().height / 2)
		{
			monsterList->removeObjectAtIndex(i);
			this->removeChild(s);
			cocos2d::log("a monster removed");
		}
	}
	for (int i = ropeList->count() - 1; i >= 0; i--)
	{
		auto s = (Sprite*)ropeList->getObjectAtIndex(i);
		backgroundPositionSub(s, dis);
		if (s->getPositionY() < -s->getContentSize().height / 2)
		{
			ropeList->removeObjectAtIndex(i);
			this->removeChild(s);
			cocos2d::log("a rope removed");
		}
	}
}

//杀怪
void GameScene::killmonster(float dt)
{
	auto pos = player->getPosition();
	Rect temp = player->getBoundingBox();
	for (int i = 0; i < monsterList->count(); i++)
	{

		Sprite* obstacleSprite = (Sprite*)monsterList->getObjectAtIndex(i);
		Rect pos = obstacleSprite->getBoundingBox();
		Rect box = Rect(pos.getMinX() - 40, pos.getMinY(), pos.getMaxX() - pos.getMinX() + 80, pos.getMaxY() - pos.getMinY());
		bool pia = box.intersectsRect(temp);
		if (pia == true)
		{
			Animation* anim = Animation::createWithSpriteFrames(monsterDead, 0.1f);
			Animate * ani = Animate::create(anim);
			if (isShield&&!isJumping) {
				this->removeChild(obstacleSprite, true);
				Sequence*seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, obstacleSprite)), NULL);
				obstacleSprite->runAction(seq);
				debuff();
				break;
			}
			
			else if ( isJumping) {
				Sequence*seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, obstacleSprite)), NULL);
				obstacleSprite->runAction(seq);
				score += 1;
				
				this->removeChild(obstacleSprite, true);
				DaZhao(1);
				monsterList->removeObjectAtIndex(i);
				killone = false;
				break;
			}
			else {	
				unscheduleAllSelectors();
				break;
			}
		}


	}
}

void GameScene::randomcreate(float dt)
{
	if ((height % 188) == 0) {
		int ran = (int)random(0, 10);
		if (ran % 2 == 0) {
			Sprite* left = Sprite::create("obstacle_l.png");
			obstacleList->addObject(left);
			left->setPosition(treeLeft->getContentSize().width / 2 - 5 + left->getContentSize().width / 2, visibleHeight + 300);

			this->addChild(left, 1);
		}
		else {
			Sprite* right = Sprite::create("obstacle_r.png");
			obstacleList->addObject(right);
			right->setPosition(visibleWidth - treeRight->getContentSize().width / 2 + 5 - right->getContentSize().width / 2, visibleHeight + 650);
			this->addChild(right, 1);
		}

	}

}

//添加障碍
void GameScene::createObstacle(float dt) {
	Sprite* left = Sprite::create("obstacle_l.png");
	Sprite* right = Sprite::create("obstacle_r.png");
	obstacleList->addObject(left);
	obstacleList->addObject(right);
	// modified
	left->setPosition(treeLeft->getContentSize().width / 2 - 5 + left->getContentSize().width / 2, visibleHeight + 300);
	right->setPosition(visibleWidth - treeRight->getContentSize().width / 2 + 5 - right->getContentSize().width / 2, visibleHeight + 600);
	// modified
	this->addChild(left, 1);
	this->addChild(right, 1);
}
//添加防护罩
void GameScene::createShield(float dt)
{
	Sprite* middle = Sprite::create("shield.png");
	shieldList->addObject(middle);
	// modified
	middle->setPosition(visibleWidth / 2, visibleHeight + 300);
	// modified
	this->addChild(middle, 1);
}
//创造怪物
void GameScene::createMonsterAndRope(float dt)
{
	auto rope = Sprite::create("rope.png");
	rope->setPosition(visibleWidth / 2, visibleHeight - 100);
	this->addChild(rope, 1);
	auto monster = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364, 0, 42, 42)));
	monster->setPosition(visibleWidth - 100, rope->getPositionY() + 30);
	this->addChild(monster, 1);
	int x = 0;
	//	isDead.pushBack(&x);
	monsterList->addObject(monster);
	ropeList->addObject(rope);
	/* monster dead animation */
	//auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	//monsterDead.reserve(4);
	//for (int i = 0; i < 4; i++) {
	//    auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
	//    monsterDead.pushBack(frame);
	//}

}
void GameScene::DaZhao(int i)
{
	
	
	std::string temp = "score: " + cocos2d::Value(score).asString();
	score_->setString(temp);
	if (score == 3) {
		ParticleFire *lizi = ParticleFire::create();
		lizi->setPosition(player->getContentSize().width / 2, player->getContentSize().height / 2);
		lizi->setRotation(180.0);
		lizi->setTag(1);
		isShield = true;
		player->addChild(lizi);
		score = 0;
		std::string temp = "score: " + cocos2d::Value(score).asString();
		score_->setString(temp);
	    schedule(schedule_selector(GameScene::BackGround),1.0/120.0);
		scheduleOnce(schedule_selector(GameScene::removeWudi),2.0f);
	}
}
void GameScene::removeWudi(float dt) {
	isShield = false;
	player->removeChildByTag(1);
	schedule(schedule_selector(GameScene::BackGround));
}
//判断碰撞
void GameScene::obstacleCollision(float dt) {

	auto pos1 = player->getPosition();
	Rect temp = Rect(pos1.x-2,pos1.y,pos1.x+2,pos1.y+2);
	for (int i = 0; i < obstacleList->count(); i++)
	{

		Sprite* obstacleSprite = (Sprite*)obstacleList->getObjectAtIndex(i);
		auto pos = obstacleSprite->getPosition();
		Rect box = Rect(pos.x - 5, pos.y, pos.x + 5, pos.y +2);
		bool pia = box.intersectsRect(temp);
		if (pia == true)
		{
			if (isShield) {
				auto *lizi = ParticleFireworks::create();
				lizi->setPosition(pos);
				lizi->setRotation(180.0);
				lizi->setName("lizi");
				this->addChild(lizi);
				scheduleOnce(schedule_selector(GameScene::removeCollisionLizi), 0.8f);
				this->removeChild(obstacleSprite, true);
				debuff();
				return;
			}
			unscheduleAllSelectors();
			break;
		}
	}
}
void GameScene::removeCollisionLizi(float) {
	this->removeChildByName("lizi");
}
void GameScene::shieldCollision(float dt) {
	auto pos = player->getPosition();
	Rect temp = player->getBoundingBox();
	for (int i = 0; i < shieldList->count(); i++)
	{
		Sprite* shieldSprite = (Sprite*)shieldList->getObjectAtIndex(i);
		Rect pos = shieldSprite->getBoundingBox();
		Rect box = Rect(pos.getMinX() - 40, pos.getMinY()-40, pos.getMaxX() - pos.getMinX() + 80, pos.getMaxY() - pos.getMinY()+80);
		bool pia = box.intersectsRect(temp);
		if (pia == true)
		{
			auto temp = shieldSprite;
			shieldList->removeObject(temp);
			this->removeChild(shieldSprite);
			debuff();//移除之前的保护盾
			buff();
		}
	}
}

void GameScene::buff() {
	auto shieldOnPlayer = Sprite::create("shield.png");
	// modified
	shieldOnPlayer->setPosition(player->getContentSize().width / 2, player->getContentSize().height / 2);
	// modified
	player->addChild(shieldOnPlayer, 0);
	isShield = true;
}
void GameScene::debuff()
{
	player->removeAllChildren();
	isShield = false;
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
		auto spawn = Spawn::createWithTwoActions(MoveTo::create(0.48f, Vec2(x, player->getPositionY())), jumpAnimate);

		bool& jumping = isJumping;
		auto seq = Sequence::create(spawn, CallFunc::create([&]() {
			player->setFlipX(true);
			auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
			player->runAction(RepeatForever::create(runAnimate));
			jumping = false;
		}), nullptr);
		player->runAction(seq);
		side = 'r';
	}
	else if (side == 'r') {
		auto x = player->getContentSize().width + 2;
		auto jumpAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaJumpAnimation"));
		auto spawn = Spawn::createWithTwoActions(MoveTo::create(0.48f, Vec2(x, player->getPositionY())), jumpAnimate);

		bool& jumping = isJumping;
		auto seq = Sequence::create(spawn, CallFunc::create([&]() {
			player->setFlipX(false);
			auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("ninjaRunAnimation"));
			player->runAction(RepeatForever::create(runAnimate));
			jumping = false;
		}), nullptr);
		player->runAction(seq);
		side = 'l';
	}

	return true;
}



void GameScene::onTouchEnded(Touch *touch, Event *event) {


}

