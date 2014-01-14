#include "ZZAnimatedSprite.h"

namespace ZZ {

AnimationLogic::AnimationLogic(void)
{

}


AnimationLogic::~AnimationLogic(void)
{

}

//static 
AnimationLogic* AnimationLogic::create( const JsonValueRef json )
{
	AnimationLogic* as = new AnimationLogic();
	as->autorelease();

	as->init( json );

	return as;
}
	
//virtual 
bool AnimationLogic::init(  const JsonValueRef json )
{
	m_states.clear();

	if( !json.isMember("states") || !json.isMember("events") ) return false;

	for( int i=0; i< json["events"].size(); i++)
	{
		m_events.push_back( json["events"][i].asString() );
	}

	m_defaultState = json.get("default", "idle").asString();

	Json::Value::Members states = json["states"].getMemberNames();
	for(int i=0; i< states.size(); i++)
	{
		JsonValueRef stateObj = json["states"][states[i]];
		Json::Value::Members events = stateObj.getMemberNames();

		AnimState animState;
		animState.name = states[i];
		CCLOG(" animState: %s", animState.name.c_str() );
		for( int j=0; j< events.size(); j++)
		{
			std::string newState = stateObj[ events[j] ].asString();
			animState.transitionTable[ events[j] ] = newState;

			CCLOG("  %s => %s", events[j].c_str(), newState.c_str() );
		}

		m_states[ states[i] ] = animState;
	}

	return true;
}

std::string AnimationLogic::getTransition( const std::string eventName, const std::string fromState )
{
	CCLOG("get transition from %s for event %s", fromState.c_str(), eventName.c_str() );

	if( fromState == "" ) return m_defaultState;
	if( m_states.count(fromState) < 1 ) return "";

	const AnimState& currAnimState = m_states[fromState];
	
	if( currAnimState.transitionTable.count( eventName ) < 1 ) return "";
	
	return currAnimState.transitionTable.at( eventName );
}


AnimatedSprite::AnimatedSprite(void)
{
	m_currAnim = NULL;
	m_animLogic = NULL;
	m_animAction = NULL;
	m_animActionSequence = NULL;
}
AnimatedSprite::~AnimatedSprite(void)
{
	CC_SAFE_RELEASE_NULL(m_currAnim);
	CC_SAFE_RELEASE_NULL(m_animLogic);

	this->stopAction( m_animActionSequence );
	CC_SAFE_RELEASE_NULL(m_animActionSequence);

	std::map<std::string, CCAnimation*>::iterator itr = m_animations.begin();
	for( ; itr != m_animations.end(); itr++)
	{
		CC_SAFE_RELEASE(itr->second);
	}
}

//static 
AnimatedSprite* AnimatedSprite::create( AnimationLogic* animLogic, JsonValueRef spriteFrames  )
{
	AnimatedSprite* sprite = new AnimatedSprite();
	sprite->init(animLogic, spriteFrames);
	sprite->autorelease();
	return sprite;

}

//virtual 
bool AnimatedSprite::init(  AnimationLogic* animLogic, JsonValueRef animSprite )
{
	m_animLogic = animLogic;
	CC_SAFE_RETAIN(m_animLogic);

	/* ex: animSprite
	{
		"texture":"derpy.png",
		"uniformFrameSize":92,
		"fps":30.0f,
		"animations":{
			"idle":[ 12,1 ],
			"walk":[ 13,2, 14,2, 15,2, 16,2, 17,2, 18,2 ],
			"run":[ 13,3, 14,3, 15,3, 16,3, 17,3, 18,3 ]
		}
	}
	*/

	std::string path = animSprite["texture"].asString();
	
	CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage( path.c_str() );
	//setTexture(texture);
	

	int uniformFrameSize = animSprite["uniformFrameSize"].asInt();
	float secondsPerFrame = 1.0f / animSprite["fps"].asDouble();

	CCRect area = CCRectMake(0,0, uniformFrameSize, uniformFrameSize);
	CCSprite::initWithTexture(texture, area);

	JsonValueRef spriteJson = animSprite["animations"];

	Json::Value::Members animSprites = spriteJson.getMemberNames();
	int numAnims = animSprites.size();

	//for each animation in set
	for( int animSetIdx=0; animSetIdx< numAnims; animSetIdx++)
	{
		std::string animName = animSprites[animSetIdx];
		JsonValueRef spriteFrames = spriteJson[ animName ];
		int numFrames = spriteFrames.size()/2;

		CCArray* sFrameList = CCArray::createWithCapacity(numFrames);
		
		

		//for each frame in the animation
		for( int currFrame=0; currFrame<numFrames; currFrame++)
		{
			//get coordinate from metadata
			int col = spriteFrames[ currFrame*2 + 0 ].asInt();
			int row = spriteFrames[ currFrame*2 + 1 ].asInt();

			

			//create a spriteFrame
			CCRect fRect = CCRectMake( uniformFrameSize*col, uniformFrameSize*row, uniformFrameSize, uniformFrameSize);

			CCLOG("%s add frame %d,%d -- %.2f,%.2fx%.2f:%.2f", animName.c_str(), col, row, fRect.origin.x, fRect.origin.y, fRect.size.width, fRect.size.height);
			CCSpriteFrame* sFrame = CCSpriteFrame::createWithTexture(texture, fRect, false, ccp(0,0), fRect.size);
			
			sFrameList->addObject(sFrame);

		
		}

		CCAnimation* animation = CCAnimation::createWithSpriteFrames(sFrameList, secondsPerFrame);
		m_animations[ animName ] = animation;
		CC_SAFE_RETAIN(animation);
	}

	/*
	m_currAnimName = m_animLogic->getDefaultState();
	if( m_animations.count( m_currAnimName ) > 0 ) 
	{
		m_currAnim = m_animations[ m_currAnimName ];
	}
	*/

	return true;
}

void AnimatedSprite::handleAnimEvent( std::string evt )
{
	std::string nextState = m_animLogic->getTransition( evt, m_currAnimName );
	setAnimState(nextState);

}

void AnimatedSprite::setAnimState( std::string stateName )
{
	if( !m_animations.count(stateName) ) {
		CCLOG("error: missing anim state %s", stateName.c_str() );
		return;
	}

	if( stateName == m_currAnimName ) 
	{
		CCLOG("anim loop %s", stateName.c_str()); //todo: see if we can reuse the ccsequence object
	}

	//stop previous anim
	if(m_animActionSequence != NULL ) {
		//m_animAction->stop();
		this->stopAction( m_animActionSequence );
		CC_SAFE_RELEASE_NULL( m_animActionSequence );
	}

	m_currAnim = m_animations[stateName];
	m_currAnimName = stateName;

	//run current anim
	m_animAction = CCAnimate::create( m_currAnim );
	
	m_animActionSequence = CCSequence::createWithTwoActions( m_animAction, CCCallFunc::create(this, callfunc_selector(AnimatedSprite::onAnimStateEnd)) );
	CC_SAFE_RETAIN(m_animActionSequence);

	CCLOG("start animation %s for %fs", stateName.c_str(), m_animAction->getDuration() );
	this->runAction(m_animActionSequence);
}

void AnimatedSprite::onAnimStateEnd()
{
	handleAnimEvent("end");
}

}

