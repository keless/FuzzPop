#ifndef _CASTCOMMANDSTATE_H_
#define _CASTCOMMANDSTATE_H_

#include <cocos2d.h>
using namespace cocos2d;

#include "CastCommandModel.h"
#include "CastEntity.h"
#include "ZZEventBus.h"

/*
CastCommandState

  carries all the stateful information of an INSTANCE of a cast command


states:
 *evt begin cast
  casting 
 *evt cast complete

 if( channeled ) 
 *evt channel start
  *evt cast/channel tick
 *evt channel end
 endif( channeled )

 *evt begin travel
  traveling
 *evt hit (can hit target, or possibly be blocked earlier in path)
  lingering (on world, on target(s))



*/

enum CCSstate {
	CCS_IDLE = 0,
	CCS_CASTING,
	CCS_CHANNELING,
	CCS_COOLDOWN,

	CCS_COUNT
};

class CastCommandState :
	public CCObject
{
	CCSstate m_state;
	double m_timeStart; //start of state at begin cast
	int m_channelTicks;
	CastCommandModel* m_pModel; //todo: use identifier instead of direct pointer?
	ICastEntity* m_iOwner;

	std::string m_costStat;
	float m_costVal;


	friend class CastEffect;

	void onCastComplete();
	void spawnCastEffects();
	void onChannelComplete();
	void spawnChannelEffects();
	void onCooldownStart();
	void onCooldownComplete();
	void onSchedulerTick(float dt);

public:
	CastCommandState(CastCommandModel* model, ICastEntity* owner);
	~CastCommandState(void);

	std::string getName() { return m_pModel->getName(); }

	
	bool canAfford();
	bool isCasting() { return m_state == CCS_CASTING; }
	float getCastPct(); //0 means 'not casting', 1.0 means 'cast complete'
	bool isChanneling() { return m_state == CCS_CHANNELING; }
	float getChannelPct(); //0 means 'not channeling' 1.0 means 'channel complete'
	bool isOnCooldown() { return m_state == CCS_COOLDOWN; }
	float getCooldownPct(); //0 means 'on cooldown', 1.0 means 'off cooldown'
	bool isIdle() { return m_state == CCS_IDLE; }
	float getRange() { return m_pModel->getRange(); }

	Json::Value getDescriptor( std::string dataName = "" );

	bool startCast();

};

#endif

