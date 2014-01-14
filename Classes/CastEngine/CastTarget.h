
#ifndef _CASTTARGET_H_
#define _CASTTARGET_H_

#include <cocos2d.h>
using namespace cocos2d;

class ICastEntity;


/*
 CastTarget 

  used to describe what a cast ability is trying to affect

*/

//TODO: LIKELY CRASH BUG:  m_entityList will hold pointers to invalid entities when they get killed

enum CastTargetType
{
	CTT_ENTITIES,			//one or more entities
	CTT_WORLD_POSITION,
	CTT_RELATIVE_POSITION,
	
	CTT_COUNT
};

class CastTarget : public CCObject
{
	CastTargetType m_type;

	CCPoint m_position;
	std::vector<ICastEntity*> m_entityList;

public:
	CastTarget(CastTargetType type = CTT_ENTITIES);
	~CastTarget(void);

	const std::vector<ICastEntity*>& getEntityList();
	CastTargetType getType() { return m_type; }
	
	void clearTargetEntities();
	void addTargetEntity( ICastEntity* target );
	void setTargetPosition( CCPoint target );

	//will purge pointers to entities that are invalid
	void validateTargets();
	bool hasTargetsAtRangeFromEntity( float range, ICastEntity* from);

	bool hasValidTarget();
	
};

#endif

