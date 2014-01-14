//
//  CastWorldModel.h
//  CastEngine
//
//  Created by Paul Zirkle on 6/5/13.
//
//

#ifndef __CastEngine__CastWorldModel__
#define __CastEngine__CastWorldModel__

#include "cocos2d.h"
using namespace cocos2d;

#include "CastTarget.h"
#include "CastEntity.h"
#include "CastEffect.h"
#include "CastPhysics.h"

/*
 CastWorldModel
 
  simulates a 'world' that casted abilities happen inside of -- 
    basically just handles effects that are "in transit" to their targets
 
 //TODO: hold all ICastEntities, then implement CastEntityHandles instead of ICastEntity*
			and resolve CastEntityHandles into ICastEntities inside of CastWorldModel
	// this will allow us to gracefully abort effects in transit to/from dead entities
	// alternately1-- hold entities in a graveyard until all references to them expire
	// alternately2-- expose "retain/release" in interface
 
 */

//TODO: FIX LIKELY CRASH BUG:  to' and from could be pointing at entities that die and delete before castPath is finished
struct CastEffectPath
{
	double startTime;
	double speed;
	
	ICastEntity* from;	
	
	ICastEntity* to;	//null if targeting world position
	kmVec2 toPosition; //use this if 'to' is null
	float radius;
	
	std::vector<CastEffect*> effects;

	//default copy constructor should be enough
};

class CastWorldModel
{
protected:
	static CastWorldModel* s_instance;
public:
	static CastWorldModel* get();
	
protected:
	std::vector<CastEffectPath> m_effectsInTransit;
	
	void applyEffectToTarget( CastEffectPath );

	std::map<ICastEntity*, ICastEntity*> m_allEntities;
	friend class ICastEntity;
	
	ICastPhysics* m_pPhysics;
public:
	CastWorldModel();
	~CastWorldModel();

	void setPhysicsInterface( ICastPhysics* physics ) { m_pPhysics = physics; }
	ICastPhysics* getPhysicsInterface() { return m_pPhysics; }
	
	void addEffectInTransit( ICastEntity* from, CastEffect* effect, CastTarget* targetList, double startTime );
	void addEffectInstant( ICastEntity* from, CastEffect* effect, CastTarget* targetList, double startTime );

	void updateStep( float dt );

	bool isValid( ICastEntity* );
};

#endif /* defined(__CastEngine__CastWorldModel__) */
