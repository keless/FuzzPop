#include "AnimationScreen.h"


AnimationScreen::AnimationScreen(void)
{
	m_pony = NULL;
}


AnimationScreen::~AnimationScreen(void)
{
	//CC_SAFE_RELEASE_NULL(m_pony);  //pony is on the scene graph, dont need to release manually
}

bool AnimationScreen::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	Json::Value data = ReadFileToJson("ponySprite.json");

	AnimationLogic* animLogic = AnimationLogic::create( data["animLogic"] );


	float menuY = origin.y + visibleSize.height/2;
	float menuX = origin.x + visibleSize.width/4;

	CCArray* items = CCArray::create();

	const std::vector<std::string> eventsList = animLogic->getEventsList();
	//for each event add a menu option
	for( int i=0; i< eventsList.size(); i++)
	{
		std::string eventName = eventsList[i];
		CCMenuItemFont* menuItem = CCMenuItemFont::create(eventName.c_str(), this, menu_selector(AnimationScreen::eventMenuCallback));
		menuItem->setTag(i);
		menuItem->setPosition(ccp( menuX, menuY ));
		menuY += menuItem->getContentSize().height * 1.25f;
		items->addObject(menuItem);
	}

	CCMenu* pMenu = CCMenu::createWithArray(items);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
	
	m_pony = AnimatedSprite::create( animLogic, data["animSprite"] );
	m_pony->setAnchorPoint(ccp(0,0));
	m_pony->setPosition(ccp( visibleSize.height/2, visibleSize.height/2 ) );
	addChild(m_pony);

	m_lblName = CCLabelTTF::create();
	m_lblName->initWithString( "state", "Arial", 24.0f  );
	m_lblName->setAnchorPoint(ccp(0.0f, 1.0f ) );
	m_lblName->setPosition(ccp( visibleSize.height/2, visibleSize.height/2 - 25 ));
	addChild(m_lblName, 50);

	m_pony->addListener("stateChange", this, callfuncO_selector(AnimationScreen::onAnimStateChange));
	m_pony->setAnimState("idle");


	setTouchEnabled(true);
	//registerWithTouchDispatcher();
    
    return true;
}

void AnimationScreen::onAnimStateChange( CCObject* e )
{
	JsonEvent* evt = dynamic_cast<JsonEvent*>(e);
	if(!evt) return;

	m_lblName->setString(evt->json["newState"].asString().c_str());
}

void AnimationScreen::eventMenuCallback(CCObject* pSender)
{
	CCMenuItemFont* menuItem = dynamic_cast<CCMenuItemFont*>(pSender);
	if(!menuItem) return;

	//todo trigger event on animation
	int idx = menuItem->getTag();
	std::string evt =  m_pony->getAnimationSet()->getEventsList()[idx];
	CCLOG("call anim event %s", evt.c_str() );
	m_pony->handleAnimEvent(evt);
}


