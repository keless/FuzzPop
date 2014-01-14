#include "CastEntity.h"

#include "CastWorldModel.h"
ICastEntity::ICastEntity()
{
	CastWorldModel::get()->m_allEntities[this] = this;
}
ICastEntity::~ICastEntity()
{
	CastWorldModel::get()->m_allEntities.erase(this);
}

