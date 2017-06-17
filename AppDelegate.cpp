#include "AppDelegate.h"
#include "GameScene.h"


USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(600, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("Jump", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
		glview = GLViewImpl::create("Jump");
#endif
		director->setOpenGLView(glview);
	}

	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::EXACT_FIT);

	// turn on display FPS
	director->setDisplayStats(false);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	FileUtils::getInstance()->addSearchPath("res");

	// create a scene. it's an autorelease object

	// load game resource
	auto spriteCache = SpriteFrameCache::getInstance();
	spriteCache->addSpriteFramesWithFile("ninja-run.plist");
	spriteCache->addSpriteFramesWithFile("ninja-jump.plist");
	char totalFrames = 24;
	char frameName[20];
	Animation* runAnimation = Animation::create();

	// load run animation
	for (int i = 0; i < totalFrames; i++)
	{
		sprintf(frameName, "ninja-run-%d.png", i);
		runAnimation->addSpriteFrame(spriteCache->getSpriteFrameByName(frameName));
	}
	runAnimation->setDelayPerUnit(0.05);
	AnimationCache::getInstance()->addAnimation(runAnimation, "ninjaRunAnimation");

	// load jump animation
	Animation* jumpAnimation = Animation::create();
	for (int i = 0; i < totalFrames; i++)
	{
		sprintf(frameName, "ninja-jump-%d.png", i);
		jumpAnimation->addSpriteFrame(spriteCache->getSpriteFrameByName(frameName));
	}
	jumpAnimation->setDelayPerUnit(0.02);
	AnimationCache::getInstance()->addAnimation(jumpAnimation, "ninjaJumpAnimation");

    auto i = AnimationCache::getInstance();

	// run
    auto scene = GameScene::createScene();
	director->runWithScene(scene);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
