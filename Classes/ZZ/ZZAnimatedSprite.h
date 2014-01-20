#ifndef _ZZANIMATIONSET_H_
#define _ZZANIMATIONSET_H_

/*
 AnimationLogic represents a data Resource (as opposed to Instance) 
	that represents the relationship of a series of animations as a state transition table

 AnimatedSprite represents an Instance and relies upon an AnimationLogi cobject

*/

#include <cocos2d.h>
using namespace cocos2d;

#include "ZZDefines.h"
#include "ZZEventBus.h"

namespace ZZ {

class AnimState
{
public:
	std::string name;

	//key = event name, value = AnimState name to move to
	std::map<std::string, std::string> transitionTable;
};

//@RESOURCE
class AnimationLogic : public CCObject
{
protected:

	//key
	std::map<std::string, AnimState> m_states;
	std::vector<std::string> m_events;

	std::string m_defaultState;

	AnimationLogic(void);
	~AnimationLogic(void);
public:

	static AnimationLogic* create( const JsonValueRef );
	virtual bool init(  const JsonValueRef json );

	const std::vector<std::string> getEventsList() { return m_events; }
	const std::string getDefaultState() { return m_defaultState; }

	std::string getTransition( const std::string eventName, const std::string fromState );

};

//@INSTANCE
class AnimatedSprite : public CCSprite, public EventBus
{
protected:

	std::map<std::string, CCAnimation*> m_animations;

	CCAnimate* m_animAction;		//@INSTANCE
	CCAction* m_animActionSequence; //@INSTANCE

	CCAnimation* m_currAnim;		//@INSTANCE
	std::string m_currAnimName;

	AnimationLogic* m_animLogic; //@RESOURCE

	AnimatedSprite(void);
	~AnimatedSprite(void);

	void onAnimStateEnd();
public:

	static AnimatedSprite* create( AnimationLogic* animLogic, JsonValueRef spriteFrames  );
	virtual bool init( AnimationLogic* animLogic, JsonValueRef animSprite );

	AnimationLogic* getAnimationSet() { return m_animLogic; }

	const std::string getCurrAnimName() { return m_currAnimName; }

	bool handleAnimEvent( std::string evt );
	bool setAnimState( std::string stateName );
};

}

#endif