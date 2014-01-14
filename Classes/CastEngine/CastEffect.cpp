
#include "CastEffect.h"

#include "CastEntity.h"

#include "CastCommandScheduler.h"

#include "CastCommandTime.h"

#include "CastWorldModel.h"

#include "CastCommandState.h"

#include "CastCommandModel.h"

typedef std::string String;

CastEffect::CastEffect()
{
	m_type = CET_DAMAGE_STAT;
	m_startTime = 0;
	m_lifeTime = 0;
	m_numTicksCompleted = 0;
	m_value = 0;
	m_tickFreq = CAST_EFFECT_TICK_FREQ;

	m_pTarget = NULL;
	m_pOrigin = NULL;

	m_pModel = NULL;
	m_modelEffectIndex = -1;
	m_isChannelEffect = false;
	m_isReturnEffect = false;
	
	this->autorelease();
}

CastEffect::~CastEffect()
{
	CC_SAFE_RELEASE_NULL(m_pModel);

	//unschedule any callbacks
	cancelTicks();

	//CCLOG("~CastEffect");
}

void CastEffect::initReturnEffect( CastEffect* parent )
{
	m_isReturnEffect = true;

	CastCommandState* originState = parent->m_pParent;
	//ICastEntity* to = parent->m_pTarget;
	ICastEntity* from = parent->m_pOrigin;
	int effectIdx = parent->m_modelEffectIndex;
	bool isChannelEffect = parent->m_isChannelEffect;
	init( originState, effectIdx, from, isChannelEffect);
}

void CastEffect::init(  CastCommandState* originState, int effectIdx, ICastEntity* from, bool isChannelEffect  )
{
	m_pParent = originState;
	m_pModel = originState->m_pModel;
	m_pModel->retain();
	m_modelEffectIndex = effectIdx;
	m_isChannelEffect = isChannelEffect;
	
	Json::Value json = getDescriptor();


	if( json.isMember("name") )
		m_name = json["name"].asString();
	else
		m_name = m_pModel->getName();

	String type = json.get("effectType", "damage").asString();
	if( type.compare("damage") == 0 ){
		m_type = CET_DAMAGE_STAT;
	}else if( type.compare("heal") == 0 ) {
		m_type = CET_HEAL_STAT;
	}else if( type.compare("buff") == 0 ) {
		m_type = CET_BUFF_STAT;
	}else if( type.compare("debuff") == 0 ) {
		m_type = CET_SUPPRESS_STAT;
	}else if( type.compare("event") == 0 ) {
		m_type = CET_SEND_EVENT;
	}

	m_pOrigin = from;

	m_isAoeEffect = json.isMember("aoeRadius");
	m_damageType = json.get("damageType", "").asString(); //ex: fire
	m_targetStat = json.get("targetStat", "").asString(); //ex: hp_curr

	m_value = json.get("valueBase", 0.0f).asDouble(); //ex: 10 damage
	//todo: handle caster stat modifiers

	m_lifeTime = json.get("effectLifetime", 0.0f).asDouble(); //ex: 1.0 seconds

	m_tickFreq = json.get("tickFreq", m_tickFreq).asDouble();
}

bool CastEffect::isPositiveEffect()
{
	if( m_type == CET_BUFF_STAT || m_type == CET_HEAL_STAT)
	{
		return true;
	}
	
	return false;
}

float CastEffect::getTravelSpeed()
{
	return (float)m_pModel->descriptor.get("travelSpeed", 0.0f).asDouble();
}

bool CastEffect::hasReturnEffect()
{
	Json::Value json = getDescriptor();

	return json.isMember("returnEffect");

}

bool CastEffect::isAoe()
{
	return m_isAoeEffect;
}


void CastEffect::setTarget( ICastEntity* target )
{
	if( !CastWorldModel::get()->isValid( target ) ) return;
	m_pTarget = target;
}

void CastEffect::startTicks()
{
	if( m_type == CET_BUFF_STAT || m_type == CET_SUPPRESS_STAT || m_type == CET_SEND_EVENT ) {
		doEffect();
	}else {
		m_numTicksCompleted = 0;
		int numTicks = (m_lifeTime / m_tickFreq) + 1;
		CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastEffect::onTick), this, m_tickFreq, numTicks, 0.0f, false);
	}

}

void CastEffect::onTick( float dt )
{
	if( !CastWorldModel::get()->isValid( m_pTarget ) ) return;

	double currTime = CastCommandTime::get();
	float delta = currTime - m_startTime;
	if( delta > m_lifeTime ) delta = m_lifeTime;

	if( m_type == CET_SUPPRESS_STAT || m_type == CET_BUFF_STAT ) {
		//handle buff/debuff end
		if( delta == m_lifeTime ) {
			CastCommandScheduler::get()->unscheduleSelector( schedule_selector(CastEffect::onTick), this );
			if( m_type == CET_BUFF_STAT ) {
				m_pTarget->endBuffProperty( m_targetStat, -1* m_value, this );
			}else {
				m_pTarget->endBuffProperty( m_targetStat, m_value, this );
			}

			m_pTarget->removeEffect(this);
		}
		
	}else {
		//handle dmg/heal over time ticks
		int numTicksPassed = delta / m_tickFreq;

		int ticksToDo = numTicksPassed - m_numTicksCompleted;
		for( int i=0; i< ticksToDo; i++) {
			doEffect();
			m_numTicksCompleted++;
		}

		if( delta >= m_lifeTime ) 
		{
			//NOTE: scheduleSelectors add references to objects, so we have to 
			// cancel our ticks before the object will be cleaned up by autopool
			cancelTicks(); 
			m_pTarget->removeEffect(this);
		}
	}


}

void CastEffect::cancelTicks()
{
	CastCommandScheduler::get()->unscheduleSelector( schedule_selector(CastEffect::onTick), this );
}

void CastEffect::doEffect()
{
	CastWorldModel* world = CastWorldModel::get();

	if( ! world->isValid( m_pTarget ) ) return;

	if( m_startTime == 0 ) m_startTime = CastCommandTime::get();

	Json::Value json = getDescriptor();

	if( json.isMember("react") ) {
		m_pTarget->handleEffectReaction( json["react"], this );
	}
	
	//CCLog("on tick %d", m_numTicksCompleted);
	switch( m_type ) 
	{
	case CET_DAMAGE_STAT:
		m_pTarget->incProperty( m_targetStat, -1* m_value, this );
		break;

	case CET_HEAL_STAT:
		m_pTarget->incProperty( m_targetStat,  m_value, this );
		break;

	case CET_SUPPRESS_STAT:
		m_pTarget->startBuffProperty( m_targetStat, -1* m_value, this );
		//CCLog("cast at %f - lifetime %f", CastCommandTime::get(), m_lifeTime );
		CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastEffect::onTick), this, m_lifeTime, 0, 0.0f, false);
		break;
				
	case CET_BUFF_STAT:
		m_pTarget->startBuffProperty( m_targetStat, m_value, this );
		CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastEffect::onTick), this, m_lifeTime, 0, 0.0f, false);
		break;

	case CET_SEND_EVENT:
		m_pTarget->handleEffectEvent( m_name, this);
		break;

	default:
		CCLOG("TODO: handle effect type");
	}

	//check for return effect


	if( json.isMember("returnEffect") )
	{
		json = json["returnEffect"];

		//validate 
		if( ! world->isValid( m_pOrigin ) ) return;

		CastEffect* bounce = new CastEffect();
		bounce->initReturnEffect(this);

		bounce->m_value = m_value;

		//swap direction
		ICastEntity* from = m_pTarget;
		ICastEntity* to = m_pOrigin;

		CastTarget* ghostTarget = new CastTarget();
		ghostTarget->addTargetEntity(to);

		world->addEffectInTransit(from, bounce, ghostTarget, CastCommandTime::get());

		CC_SAFE_RELEASE_NULL(ghostTarget);
	}


}

Json::Value CastEffect::getDescriptor( std::string name )
{
	if( m_pModel == NULL || m_modelEffectIndex < 0 ) return Json::Value();

	Json::Value json;
	if( m_isChannelEffect ) {
		json = m_pModel->getEffectOnChannel(m_modelEffectIndex);
	}else {
		json = m_pModel->getEffectOnCast(m_modelEffectIndex);
	}

	if( m_isReturnEffect )
	{
		json = json.get("returnEffect", Json::Value());
	}

	if( name.size() > 0 ) {
		json = json.get(name, Json::Value() );
	}

	return json;
}

CastEffect* CastEffect::clone()
{
	CastEffect* effect = new CastEffect();


	effect->m_type = m_type;
	effect->m_startTime = m_startTime;
	effect->m_lifeTime = m_lifeTime;
	effect->m_tickFreq = m_tickFreq;
	effect->m_damageType = m_damageType;
	effect->m_targetStat = m_targetStat;
	effect->m_stackFlag = m_stackFlag;
	effect->m_numTicksCompleted = m_numTicksCompleted;
	effect->m_value = m_value;
	effect->m_pTarget = m_pTarget;
	effect->m_pOrigin = m_pOrigin;
	effect->m_pModel = m_pModel;
	effect->m_pParent = m_pParent;
	effect->m_modelEffectIndex = m_modelEffectIndex;
	effect->m_isChannelEffect = m_isChannelEffect;
	effect->m_isReturnEffect = m_isReturnEffect;
	effect->m_name = m_name;

	return effect;
}