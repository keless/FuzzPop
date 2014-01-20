#ifndef _ENTITYANIMCONTROLLER_H_
#define _ENTITYANIMCONTROLLER_H_

/***************
	EntityAnimController class represents the glue between animation logic and entity state logic
		an action impluse may try to 'jump' or 'turn left' while the animation is busy in the 'landing' animation
	@RESOURCE -- EntityAnimController represents a Resource, and can be used on multiple Instance objects

	bool attemptCommand( std::string entityImpulseCmd, AnimatedSprite* onSprite  )
		when an impulse comes in, we map that impulse to an animation event and see if it can be started
		on the given sprite
		if it can, we start it and return true
		if not, we return false

	void setImpulseMapping( std::string entityImpulseCmd, std::string animEvent )
		before using the animController, we will call setImpulseMapping for each action impulse that we handle

****************/


#include "ZZUtils.h"
using namespace ZZ;

//@INSTANCE
class EntityAnimController : public CCObject
{
	AnimationLogic* m_logic; //@RESOURCE

	std::map<std::string, std::string> m_impulseMap;

public:
	EntityAnimController(AnimationLogic* animLogic);
	~EntityAnimController(void);

	void setImpulseMapping( std::string entityImpulseCmd, std::string animEvent );

	bool attemptCommand( std::string entityImpulseCmd, AnimatedSprite* onSprite );
};

#endif
