#include "MainMenuScreen.h"


MainMenuScreen::MainMenuScreen(void)
{
}


MainMenuScreen::~MainMenuScreen(void)
{

}

// on "init" you need to initialize your instance
bool MainMenuScreen::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	float menuY = origin.y + visibleSize.height/2;
	float menuX = origin.x + visibleSize.width/2;

	CCMenuItemFont* menuTravel = CCMenuItemFont::create("Play", this, menu_selector(MainMenuScreen::menuStartCallback));
	menuTravel->setPosition(ccp( menuX, menuY));


	menuY += menuTravel->getContentSize().height;

	CCMenuItemFont* menuParty = CCMenuItemFont::create("Anim Viewer", this, menu_selector(MainMenuScreen::menuAnimViewerCallback));
	menuParty->setPosition(ccp( menuX, menuY ));

	menuY += menuTravel->getContentSize().height;

	CCMenu* pMenu = CCMenu::create(menuTravel, menuParty, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);


	setTouchEnabled(true);
	//registerWithTouchDispatcher();
    
    return true;
}

#include "ZZEventBus.h"
using namespace ZZ;
void MainMenuScreen::menuStartCallback(CCObject* pSender)
{
	BaseEvent* evt = new BaseEvent("battle");
	EventBus::get("state")->dispatch("switchTo", evt);

}
void MainMenuScreen::menuAnimViewerCallback(CCObject* pSender)
{
	BaseEvent* evt = new BaseEvent("animViewer");
	EventBus::get("state")->dispatch("switchTo", evt);
}
