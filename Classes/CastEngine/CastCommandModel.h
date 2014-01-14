#ifndef _CASTCOMMANDMODEL_H_
#define _CASTCOMMANDMODEL_H_

#include <cocos2d.h>
using namespace cocos2d;

#include "json.h"

/*
 CastCommandModel

   carries shared resource information used to create CastCommandState objects from
     ie: one CastCommandModel represents "fireball", but three mages would each have their own CCS that points to the same CCM

*/

//NOTE: all times are in seconds (milliseconds are decimal)

class CastCommandModel :
	public CCObject
{
	std::string name;

	//base values, unmodified by buff/debufs (which happens at time of cast)
	float castTime;		//zero if instant 
	float channelTime;	//zero if not channelled
	float channelFreq;	//tick frequency of channeling		
	float travelSpeed;	//zero if instant (units per second, where 1 unit is melee range, 2 units is polearm range, and 3+ units is 'ranged attack')
	float range;		//distance it can travel
	bool effectWhileTraveling; //true if can cause effect while travelling (might be immediately consumed: fireball; or continue to effect while travelling: lava wall)
	bool stopOnHit;

	//float travelDistance; //instance data
	float cooldownTime; //time in MS after castEnd before castBegin can start again

	float effectSize; //zero if no physics involved
	int effectShape; //singleTarget, line, cone, circle, etc


	//debug:
	Json::Value descriptor;

public:
	CastCommandModel( const Json::Value& castData );
	~CastCommandModel(void);

	bool isChanneled() { return channelTime > 0; }
	bool isInstant() { return castTime <= 0; }

	int getNumEffectsOnChannel(); //per tick
	Json::Value getEffectOnChannel( int idx );

	int getNumEffectsOnCast();
	Json::Value getEffectOnCast( int idx );

	float getRange() { return range; }

	std::string getName() { return name; }

	friend class CastCommandState;
	friend class CastEffect;
};

#endif
