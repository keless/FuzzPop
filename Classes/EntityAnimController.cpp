#include "EntityAnimController.h"


EntityAnimController::EntityAnimController(AnimationLogic* animLogic)
{
	m_logic = animLogic;
}


EntityAnimController::~EntityAnimController(void)
{
}

void EntityAnimController::setImpulseMapping( std::string entityImpulseCmd, std::string animEvent )
{
	m_impulseMap[ entityImpulseCmd ] = animEvent;
}

bool EntityAnimController::attemptCommand( std::string entityImpulseCmd, AnimatedSprite* onSprite )
{
	if( m_impulseMap.count( entityImpulseCmd ) < 1 ) return false; //could not find command

	const std::string& animEvent = m_impulseMap[ entityImpulseCmd ];

	return onSprite->handleAnimEvent( animEvent );
}

