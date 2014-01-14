#ifndef _CASTEFFECT_H_
#define _CASTEFFECT_H_


#include <cocos2d.h>
using namespace cocos2d;

#include "json.h"

#define CAST_EFFECT_TICK_FREQ 1.0f  //default one per second

enum CastEffectType
{
	CET_DAMAGE_STAT,	//decrements stats that can be decremented (health, mana, etc)
	CET_SUPPRESS_STAT,	//temporarily decrements stats while in effect (str, agi, etc)
	CET_HEAL_STAT,		//increments stats that can be incremented
	CET_BUFF_STAT,		//temporarily increases stats while in effect
	CET_SEND_EVENT,		//causes an event to be sent to the game bus
};

/*
 
 effects supported:
   * 
 
 todo support:
   * effects that spawn more effects that jump to nearby targets
   * effects that target nearby targets
   * stacking buffs/debuffs (game code specific?)
   * 
 
 */

class ICastEntity;
class CastCommandState;
class CastCommandModel;

class CastEffect : public CCObject
{

	//time started, time lasting

	CastEffectType m_type;
	double m_startTime;
	double m_lifeTime; //0.0f if instant effect
	float m_tickFreq;

	bool m_isChannelEffect; //look for json under effectsOnChannel instead of effectsOnCast
	bool m_isReturnEffect;
	bool m_isAoeEffect;

	std::string m_name;
	std::string m_damageType;
	std::string m_targetStat;
	std::string m_stackFlag; //helper for stacking multiple effects

	int m_numTicksCompleted;

	float m_value; //how much damage, or how much suppression, or how much buff, etc

	//TODO: FIX LIKELY CRASH BUG: when pointers to entities go out of scope
	ICastEntity* m_pTarget;
	ICastEntity* m_pOrigin;

	CastCommandModel* m_pModel;
	CastCommandState* m_pParent;
	int m_modelEffectIndex; //index into list of effects held by CastCommandModel

	void initReturnEffect( CastEffect* parent );

public:
	CastEffect();
	virtual ~CastEffect();

	
	void init( CastCommandState* originState, int effectIdx, ICastEntity* from, bool isChannelEffect  );
	void setTarget( ICastEntity* to );

	void startTicks();

	std::string getName() { return m_name; }
	CastEffectType getType() { return m_type; }
	double getStartTime() { return m_startTime; }

	ICastEntity* getTarget() { return m_pTarget; }
	ICastEntity* getOrigin() { return m_pOrigin; }

	double getLifeTime() { return m_lifeTime; }
	double getElapsedTime( double currTime ) { return currTime - m_lifeTime; }
	bool isPositiveEffect(); //is this a nice buff, or a mean debuff?

	void initReturn();
	bool hasReturnEffect();
	bool isAoe();
	float getTravelSpeed();

	void onTick( float dt );
	void cancelTicks();

	void doEffect();

	Json::Value getDescriptor( std::string name = "" );

	CastEffect* clone();

	friend class CastWorldModel;
};

#endif
