#ifndef _ZZANIMATIONSET_H_
#define _ZZANIMATIONSET_H_

/*
 ZZAnimationSet represents a data Resource (as opposed to Instance) 
	that represents the relationship of a series of animations as a state transition table


*/

#include <cocos2d.h>
using namespace cocos2d;

#include "ZZDefines.h"

namespace ZZ {

class AnimState
{
public:
	std::string name;

	//key = event name, value = AnimState name to move to
	std::map<std::string, std::string> transitionTable;
};

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

class AnimatedSprite : public CCSprite
{
protected:

	std::map<std::string, CCAnimation*> m_animations;

	CCAnimate* m_animAction;
	CCAction* m_animActionSequence;

	CCAnimation* m_currAnim;
	std::string m_currAnimName;

	AnimationLogic* m_animLogic;

	AnimatedSprite(void);
	~AnimatedSprite(void);

	void onAnimStateEnd();
public:

	static AnimatedSprite* create( AnimationLogic* animLogic, JsonValueRef spriteFrames  );
	virtual bool init( AnimationLogic* animLogic, JsonValueRef animSprite );

	AnimationLogic* getAnimationSet() { return m_animLogic; }

	void handleAnimEvent( std::string evt );
	void setAnimState( std::string stateName );
};

}

#endif