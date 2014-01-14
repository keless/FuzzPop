#include "GameEntityView.h"


GameEntityView::GameEntityView( GameEntity* entity, JsonValueRef animSpriteJson )
{
	m_pEntity = entity;
	m_pEntity->retain();

	m_pEntity->addListener("react", this, callfuncO_selector(GameEntityView::onShouldReact) );
	m_pEntity->addListener("incProperty", this, callfuncO_selector(GameEntityView::onStatUpdate) );

	AnimationLogic* animLogic = m_pEntity->getAnimLogic();
	AnimatedSprite::init(animLogic, animSpriteJson);

	//TODO: synch animatedSprite up with GameEntity

	//initView();

	//scheduleUpdate();
}

//virtual 
void GameEntityView::initView()
{
	/*
	int cardW = 200;
	int cardH = 200;
	setContentSize( CCSizeMake( cardW, cardH ));
	setAnchorPoint(ccp(0.5f,0.5f));

	CCLayerColor* bg = CCLayerColor::create(ccc4(25,25,25,255), cardW,cardH);
	bg->setAnchorPoint(ccp(0,0));
	addChild(bg, 1);

	int margin = 2;
	m_highlight = CCLayerColor::create(ccc4(200,200,20,255), cardW+margin*2,cardH+margin*2);
	m_highlight->setPosition(ccp(-margin,-margin));
	m_highlight->setAnchorPoint(ccp(0,0));
	addChild(m_highlight, 0);
	setHighlighted(false);

	m_lblName = CCLabelTTF::create();
	m_lblName->initWithString( m_pEntity->getName().c_str(), "Arial", 24.0f  );
	m_lblName->setAnchorPoint(ccp(0.0f, 1.0f ) );
	m_lblName->setPositionY(cardH);
	addChild(m_lblName, 50);

	int healthBarH = 30;
	m_healthBar = ProgressBar::create( CCRectMake(0,0, cardW/2, healthBarH) );
	m_healthBar->setMargin(margin);
	m_healthBar->setProgress(  m_pEntity->getProperty("hp_curr") / (m_pEntity->getProperty("hp_base")) );
	m_healthBar->setAnchorPoint(ccp(0.0f, 1.0f) );
	m_healthBar->setPositionY( 200 - m_lblName->getContentSize().height );
	addChild(m_healthBar, 49);

	int manaBarH = 10;
	m_manaBar = ProgressBar::create( CCRectMake( 0,0, cardW/2, manaBarH ) );
	m_manaBar->setMargin(margin);
	m_manaBar->setColor( ccc3(50,50,255), ccc3(50,50,50) );
	m_manaBar->setProgress( m_pEntity->getProperty("mana_curr") / (m_pEntity->getProperty("mana_base")) );
	m_manaBar->setAnchorPoint(ccp(0,1.0f));
	m_manaBar->setPositionY( m_healthBar->getPositionY() - (m_healthBar->getContentSize().height + margin*2) );
	addChild(m_manaBar, 48);

	int xpBarH = 10;
	m_xpBar = ProgressBar::create( CCRectMake( 0,0, cardW, xpBarH ) );
	m_xpBar->setMargin(margin);
	m_xpBar->setColor( ccc3(150,150,150), ccc3(20,20,20) );

	if(  m_pEntity->getProperty("xp_next") > 0 ) {
		m_xpBar->setProgress( m_pEntity->getProperty("xp_curr") / m_pEntity->getProperty("xp_next") );
	}else {
		m_xpBar->setVisible(false);
	}
	m_xpBar->setAnchorPoint( ccp(0, 1.0f) );
	m_xpBar->setPositionY( m_manaBar->getPositionY() - (m_manaBar->getContentSize().height + margin*2) );
	addChild(m_xpBar, 48);


	m_lblLevel = CCLabelTTF::create();
	m_lblLevel->initWithString( m_pEntity->getLevelStr().c_str(), "Arial", 10.0f );
	m_lblLevel->setAnchorPoint( ccp(0.0f, 1.0f ) );
	m_lblLevel->setPositionY( m_xpBar->getPositionY() + xpBarH/2 );
	addChild(m_lblLevel, 49);
	
	int bufIndSize = cardW/6;
	m_buffIndicator = CCLayerColor::create(ccc4(25,200,25,255), bufIndSize,bufIndSize);
	//m_buffIndicator->setAnchorPoint(ccp(0.0f,0.0f));
	m_buffIndicator->setPositionX(cardW - bufIndSize);
	m_buffIndicator->setPositionY(cardH - bufIndSize);
	m_buffIndicator->setVisible(false);
	addChild(m_buffIndicator, 10);
	

	m_debuffIndicator = CCLayerColor::create(ccc4(200,25,25,255), bufIndSize,bufIndSize);
	//m_debuffIndicator->setAnchorPoint(ccp(0,0));
	m_debuffIndicator->setPositionX(cardW - (bufIndSize*2 + margin));
	m_debuffIndicator->setPositionY(cardH - bufIndSize);
	m_debuffIndicator->setVisible(false);
	addChild(m_debuffIndicator, 10);
	
	 
	std::vector<CastCommandState*>& abilities = m_pEntity->getAbilityList();
	int maxPerRow = 2;
	int row = 0;
	int col = 0;
	for( int i=0; i < abilities.size(); i++)
	{
		col = (i/maxPerRow);
		row = i - (col*maxPerRow);

		CastCommandState* ability = abilities[i];

		GameAbilityView* abilityView = new GameAbilityView( ability );
		m_abilityViews.push_back(abilityView);

		abilityView->setPositionX( col * abilityView->getContentSize().width );
		abilityView->setPositionY( row * abilityView->getContentSize().height );
		addChild(abilityView, 51);
	}
	*/
}


GameEntityView::~GameEntityView(void)
{
	CCLog("~GameEntityView(%s)", m_lblName->getString());
	detatchFromEntity();
}

void GameEntityView::detatchFromEntity()
{
	if( m_pEntity != NULL ) {
		m_pEntity->remListener("react", this, callfuncO_selector(GameEntityView::onShouldReact) );
		m_pEntity->remListener("incProperty", this, callfuncO_selector(GameEntityView::onStatUpdate) );
		CC_SAFE_RELEASE_NULL(m_pEntity);
	}
}

void GameEntityView::setBackground( std::string imgName )
{
	CCSize size = this->getContentSize();
	CCTexture2D* tex =  CCTextureCache::sharedTextureCache()->addImage( imgName.c_str() );
	CCSprite* spriteBG = CCSprite::createWithTexture(tex);
	spriteBG->setScaleX( size.width / spriteBG->getContentSize().width );
	spriteBG->setScaleY( size.height / spriteBG->getContentSize().height );
	spriteBG->setAnchorPoint(ccp(0,0));
	addChild(spriteBG, 5);
}

void GameEntityView::onStatUpdate(CCObject* e)
{
	updateView();
}

void GameEntityView::doShake()
{
	//CCLog("shake entity view ");
#define SHAKE_TAG 0xDEADBEEF
	if( getActionByTag(SHAKE_TAG) == NULL ) {
		CCSequence* shake = CCSequence::create(
			CCRotateBy::create(0.1f, 10, 0),
			CCRotateBy::create(0.2f, -20, 0),
			CCRotateBy::create(0.1f, 10, 0), 
			NULL);
		shake->setTag(SHAKE_TAG);

		runAction(shake);
	}
}

void GameEntityView::doBurn()
{
	//CCLog("particle system burn");

	CCParticleFire* fire = CCParticleFire::createWithTotalParticles(150);
	CCPoint point = CCPointMake(  m_healthBar->getContentSize().width/2, 0 );

	//fire->stopSystem();
	fire->setSpeed(20);
	fire->setStartSize(25);
	CCPoint posVar = fire->getPosVar();
	posVar.y /= 2;
	fire->setPosVar( posVar );

	fire->setPosition( point );
	m_healthBar->addChild(fire);

	CCSequence* seq = CCSequence::create(
					CCDelayTime::create(0.5f),
					CCCallFunc::create(fire, callfunc_selector(CCParticleFire::stopSystem)),
					CCDelayTime::create(2.5f),
					CCRemoveSelf::create(true),
					NULL
				);

	fire->runAction(seq);
}

void GameEntityView::doLifedrain( ICastEntity* to )
{
	CCParticleFire* life = CCParticleFire::createWithTotalParticles(25);
	CCPoint point =  m_healthBar->boundingBox().origin;

	life->setStartColor( ccc4f(0,1.0f,0,1.0f) );
	life->setEndColor( ccc4f(0,1.0f,0,1.0f) );
	life->setPosition(ccp(0,0));

	//TODO: use physics interface to get position of 'to' entity and direct stream

	life->setGravity( CCPointMake( -30, 0 ) );
	life->setStartSize(25);
	life->setPosVar( CCPointMake( 10,10 ));

	m_healthBar->addChild(life);

	CCSequence* seq = CCSequence::create(
				CCDelayTime::create(0.5f),
				CCCallFunc::create(life, callfunc_selector(CCParticleFire::stopSystem)),
				CCDelayTime::create(2.5f),
				CCRemoveSelf::create(true),
				NULL
			);

	life->runAction(seq);
}

void GameEntityView::doHeal()
{
	CCParticleFire* heal = CCParticleFire::createWithTotalParticles(25);
	CCPoint point = CCPointMake(  m_healthBar->getContentSize().width/2, 0 );

	heal->setStartColor( ccc4f(1,1,1,1.0f) );
	heal->setEndColor( ccc4f(1,1,1,1.0f) );

	heal->setSpeed(10);
	heal->setStartSize(15);
	CCPoint posVar = heal->getPosVar();
	posVar.y /= 2;
	heal->setPosVar( posVar );

	heal->setPosition( point );
	m_healthBar->addChild(heal);

	CCSequence* seq = CCSequence::create(
					CCDelayTime::create(0.5f),
					CCCallFunc::create(heal, callfunc_selector(CCParticleFire::stopSystem)),
					CCDelayTime::create(1.5f),
					CCRemoveSelf::create(true),
					NULL
				);

	heal->runAction(seq);
}


void GameEntityView::onShouldReact(CCObject* e)
{
	GameEntityReactEvt* evt = dynamic_cast<GameEntityReactEvt*>(e);
	if( evt == NULL ) return;

	//react to effect

	if( evt->react.isString() ) {
		std::string react = evt->react.asString();
		if( react.compare("shake") == 0 ) {
			doShake();
		}else if( react.compare("burn") == 0 ) {
			doBurn();
		}else if( react.compare("lifedrain") == 0 ) {
			doLifedrain( evt->source->getOrigin() );
		}else if( react.compare("heal") == 0 ) {
			doHeal();
		}
	}
}

void GameEntityView::setHighlighted( bool highlight )
{
	m_highlight->setVisible(highlight);
}

void GameEntityView::updateView()
{
	m_lblName->setString( m_pEntity->getName().c_str() );
	m_healthBar->setProgress(  (m_pEntity->getProperty("hp_curr")) / (float) (m_pEntity->getProperty("hp_base")) );
	m_manaBar->setProgress(  (m_pEntity->getProperty("mana_curr")) / (float) (m_pEntity->getProperty("mana_base")) );
	if(  m_pEntity->getProperty("xp_next") > 0 ) {
		m_xpBar->setProgress( m_pEntity->getProperty("xp_curr") / m_pEntity->getProperty("xp_next") );
	}
	m_lblLevel->setString( m_pEntity->getLevelStr().c_str() );
	
	m_buffIndicator->setVisible( m_pEntity->numBuffs() > 0 );
	m_debuffIndicator->setVisible( m_pEntity->numDebuffs() > 0 );
}

bool GameEntityView::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
	return boundingBox().containsPoint( touch->getLocationInView() );
}

void GameEntityView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	/*
	for( int i=0; i< m_abilityViews.size(); i++) {
		m_abilityViews[i]->ccTouchMoved(touch, event);
	}
	*/
}

void GameEntityView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
	CCPoint p = touch->getLocation();

	if( !boundingBox().containsPoint( p ) ) return;

	CCLog("game entity %s touched", m_pEntity->getName().c_str() );

	p.x -= getPositionX();
	p.y -= getPositionY();

	//activate ability if possible

	//todo: see if we can activate this ability or not (cant cast multiple abilities at the same time!)

	//dont send touch if cant cast
	if( ! m_pEntity->canCast() ) return;

	/*
	for( int i=0; i< m_abilityViews.size(); i++) {
		GameAbilityView* view = m_abilityViews[i];

		//CCRect btn = view->boundingBox();
		
		if( !view->boundingBox().containsPoint( p ) ) continue;

		if( view->canCast() ) {
			view->startCast();
		}
		return;
	}
	*/


}
void GameEntityView::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{

}