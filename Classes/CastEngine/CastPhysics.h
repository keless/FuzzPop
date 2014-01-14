#ifndef _CASTPHYSICS_H_
#define _CASTPHYSICS_H_

#include "CastEntity.h"
#include <kazmath/kazmath.h>

class ICastPhysics
{
public:

	virtual bool GetVecBetween( ICastEntity* from, ICastEntity* to, kmVec2& distVec ) = 0;
	virtual bool GetEntityPosition( ICastEntity* entity, kmVec2& pos ) = 0;
	virtual bool GetEntitiesInRadius( kmVec2 p, float r, std::vector<ICastEntity*>& entities ) = 0;

};

#endif
