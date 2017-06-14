#include "cocos2d.h"
using namespace cocos2d;

using std::string;

class GameScene : public cocos2d::Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	void BackGround(float dt);

	void addTouchListener();
	void createObstacle(float dt);
	void collision(float dt);
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);


	char side;//记录上一次player所在边

	// implement the "static create()" method manually
	CREATE_FUNC(GameScene);



private:
	float visibleHeight;
	float visibleWidth;
	cocos2d::Sprite* treeLeft;
	cocos2d::Sprite* treeLeft1;
	cocos2d::Sprite* treeRight;
	cocos2d::Sprite* treeRight1;
	cocos2d::Sprite* treeRootLeft;
	cocos2d::Sprite* treeRootRight;
	cocos2d::Sprite* scenery;
	cocos2d::Sprite* player;
	cocos2d::Sprite* sky;
	Animate* jumpAnimate;
	Animate* runAnimate;
	bool isJumping;
	int score;
	cocos2d::Label* score_;
	Array* obstacleList;//障碍物
};
