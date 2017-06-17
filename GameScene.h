#include "cocos2d.h"
#include "ui/CocosGUI.h"
using namespace cocos2d::ui;
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
    void createShield(float dt);
	void obstacleCollision(float dt);
	void shieldCollision(float dt);
	bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
	void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void menuChangeitem(Ref * pSender);
	void buff();//player with shield
	void debuff();//player without shield

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
    Button* button;
	Animate* jumpAnimate;
	Animate* runAnimate;
	bool isJumping;
    bool isShield;
	int score;
	cocos2d::Label* score_;
    cocos2d::Sprite* CurrentShield;
	Array* obstacleList;//障碍
    Array* shieldList;//保护罩
};
