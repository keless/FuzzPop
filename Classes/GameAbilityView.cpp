#include "GameAbilityView.h"


GameAbilityView::GameAbilityView(CastCommandState* ability)
{
	m_pAbility = ability;
	m_pAbility->retain();

	int w = 100;
	int h = 50;

	setContentSize( CCSizeMake(w, h) );

	CCLayerColor* bg = CCLayerColor::create(ccc4(200,250,20,255), w, h);
	addChild(bg);

	float margin = 2;
	bg = CCLayerColor::create(ccc4(0,250,0,255), w - margin*2, h - margin*2);
	//bg->setAnchorPoint(ccp(0.5f,0.5f));
	bg->setPosition(margin, margin);
	addChild(bg);

	m_castOverlay = CCLayerColor::create(ccc4(30,30,200, 255/3), w, h);
	m_castOverlay->setAnchorPoint(ccp(0,0));
	m_castOverlay->setScaleX(0.0f);
	addChild(m_castOverlay);

	m_lblName = CCLabelTTF::create( m_pAbility->getName().c_str(), "Arial", 12.0f);
	m_lblName->setAnchorPoint(ccp(0,0.5f));
	m_lblName->setPositionY(h/2);
	m_lblName->setPositionX(10);
	addChild(m_lblName);

	m_cdOverlay = CCLayerColor::create(ccc4(0,0,0, 255 / 4 ), w, h );
	m_cdOverlay->setAnchorPoint(ccp(0,0));
	//m_cdOverlay->setScaleY(0.501f);
	addChild(m_cdOverlay);



	scheduleUpdate();
}


GameAbilityView::~GameAbilityView(void)
{
	m_pAbility->release();
}

bool GameAbilityView::canCast()
{
	return m_pAbility->isIdle();
}

bool GameAbilityView::startCast()
{
	return m_pAbility->startCast();
}

void GameAbilityView::update( float delta )
{
	if( m_pAbility->isOnCooldown() ) {
		float cooldown = 1.0f - m_pAbility->getCooldownPct();
		m_cdOverlay->setScaleY( cooldown );
	}else {
		m_cdOverlay->setScaleY(0);
	}

	if( m_pAbility->isCasting() ) {

		float cast = m_pAbility->getCastPct();
		m_castOverlay->setScaleX( cast );
	}else if(  m_pAbility->isChanneling()  ) {
		float channel = m_pAbility->getChannelPct();
		m_castOverlay->setScaleX( 1.0f - channel );
	}else {
		m_castOverlay->setScaleX(0);
	}


}

inline CCPoint locationInGLFromTouch(CCTouch& touch)
{
   CCDirector* director = CCDirector::sharedDirector();
   return director->convertToGL(touch.getLocationInView());
}


bool GameAbilityView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	return boundingBox().containsPoint( touch->getLocationInView() );
}

void GameAbilityView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	
}

void GameAbilityView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
	if( !boundingBox().containsPoint( touch->getLocation() ) ) return;

	//activate ability if possible

	//todo: see if we can activate this ability or not (cant cast multiple abilities at the same time!)

	if( m_pAbility->isIdle() ) {
		m_pAbility->startCast();
	}

}
void GameAbilityView::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{

}