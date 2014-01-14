#ifndef _CASTCOMMANDSCHEDULER_H_
#define _CASTCOMMANDSCHEDULER_H_

#include <cocos2d.h>
using namespace cocos2d;

#include "CastCommandModel.h"
#include "ZZEventBus.h"

/*
 CastCommandScheduler

 used by CastCommandStates to schedule callbacks and timer ticks
*/

class CastCommandScheduler :
	public CCScheduler
{
protected:
	static CastCommandScheduler* s_instance;
public:
	static CastCommandScheduler* get();

public:
	CastCommandScheduler(void);
	~CastCommandScheduler(void);
};

#endif
