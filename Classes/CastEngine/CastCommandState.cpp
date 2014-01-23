#include "CastCommandState.h"

#include "CastCommandTime.h"

#include "CastCommandScheduler.h"

#include "CastEffect.h"

#include "CastWorldModel.h"

CastCommandState::CastCommandState(CastCommandModel* model, ICastEntity* owner)
{
	m_iOwner = owner;
	m_pModel = model;
	m_pModel->retain();

	m_state = CCS_IDLE;
	m_timeStart = 0;

	m_costStat = model->descriptor.get("costStat", "").asString();
	m_costVal = model->descriptor.get("costVal", 0).asDouble();

	m_channelTicks = 0;
}


CastCommandState::~CastCommandState(void)
{
	m_pModel->release();
}

bool CastCommandState::canAfford()
{
	if( m_costVal == 0 ) return true;

	float val = m_iOwner->getProperty(m_costStat);
	return (val >= m_costVal);
}

float CastCommandState::getCastPct()
{
	if( m_state == CCS_CASTING ) {
		double currTime = CastCommandTime::get();
		float delta = (float)(currTime - m_timeStart);
		if( delta > m_pModel->castTime ) delta = m_pModel->castTime;  //TODO: support cast speed increase
		if( delta < 0 ) delta = 0;
		return delta / m_pModel->castTime;
	}else {
		return 0.0f;
	}
}

float CastCommandState::getChannelPct()
{
	if( m_state == CCS_CHANNELING ) {
		double currTime = CastCommandTime::get();
		float delta = (float)(currTime - m_timeStart);
		if( delta > m_pModel->channelTime ) delta = m_pModel->channelTime;  //TODO: support cast speed increase
		if( delta < 0 ) delta = 0;
		return delta / m_pModel->channelTime;
	}else {
		return 0.0f;
	}
}


float CastCommandState::getCooldownPct()
{

	if( m_state == CCS_COOLDOWN ) {
		double currTime = CastCommandTime::get();
		float delta = (float)(currTime - m_timeStart);
		if( delta > m_pModel->cooldownTime ) delta = m_pModel->cooldownTime; //TODO: support cooldown decrease
		if( delta < 0 ) delta = 0;
		return delta / m_pModel->cooldownTime;
	}else {
		return 1.0f;
	}
}

bool CastCommandState::startCast()
{
	if( ! isIdle() ) return false;

	m_state = CCS_CASTING;
	m_timeStart = CastCommandTime::get();
	m_channelTicks = 0;

	if( m_pModel->castTime == 0 ) {
		//handle instant cast
		onSchedulerTick(0.0f);

	}else {

		//TODO: should we set castTime as delay instead of interval?
		CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this, m_pModel->castTime, 0, 0.0f, false);
	}

	return true;
}

void CastCommandState::onSchedulerTick( float dt)
{
	

	double currTime = CastCommandTime::get();
	float delta = (float)(currTime - m_timeStart);

	//CCLog("Cast %s tick : delta %.2f", m_pModel->getName().c_str(), delta); 

	if( m_state == CCS_CASTING ) 
	{
		if( delta >= m_pModel->castTime )  //TODO: handle cast speed increase
		{ 
			//casting complete
			CastCommandScheduler::get()->unscheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this );
			onCastComplete();
		}
	}
	else if( m_state == CCS_CHANNELING ) {
		//TODO: handle channeling ticks
		if( delta > m_pModel->channelTime ) delta = m_pModel->channelTime;

		int numTicksPassed = delta / m_pModel->channelFreq;
		int ticksToDo = numTicksPassed - m_channelTicks;
		for( int i=0; i< ticksToDo; i++ ) {
			//do tick
			spawnChannelEffects();
			//CCLOG("channel %s tick", m_pModel->getName().c_str());
			m_channelTicks++;
		}

		if( delta >= m_pModel->channelTime ) {
			// cancel callback
			CastCommandScheduler::get()->unscheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this );
			onChannelComplete();
		}
		
	}
	else if( m_state == CCS_COOLDOWN ) 
	{
		if( delta >= m_pModel->cooldownTime ) //TODO: handle cooldown redux
		{
			CastCommandScheduler::get()->unscheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this );
			onCooldownComplete();
		}
	}
}

void CastCommandState::onChannelComplete()
{
	if( m_state != CCS_CHANNELING ) return;
	if( !CastWorldModel::get()->isValid( m_iOwner ) ) return;

	//CCLog("channel complete");

	onCooldownStart();
}

void CastCommandState::onCooldownStart()
{
	m_state = CCS_COOLDOWN; //TODO: handle cooldown redux?
	CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this, m_pModel->cooldownTime, 0, 0.0f, false);

}

void CastCommandState::spawnChannelEffects()
{
	//CCLog("spawn channel tick effect");

		//spawn effects
	CastTarget* target = m_iOwner->getTarget();
	target->validateTargets();

	
	for( int i=0; i< m_pModel->getNumEffectsOnChannel(); i++ )
	{
		CastEffect* effect = new CastEffect( );
		effect->init(this, i, m_iOwner, true );
		
		//TODO: send all effects as one array so only one "packet" has to travel

		CastWorldModel* world = CastWorldModel::get();
		world->addEffectInTransit(m_iOwner, effect, m_iOwner->getTarget(), CastCommandTime::get());
	}
}

void CastCommandState::onCastComplete()
{
	if( m_state != CCS_CASTING ) return;

	CastWorldModel* world = CastWorldModel::get();
	if( !world->isValid( m_iOwner ) ) return;

	//check for cost (but dont apply it yet)
	if( m_costVal != 0 ) {
		float res = m_iOwner->getProperty( m_costStat );

		//checking cost>0 so that if a tricky user wants cost to be 'negative' to 'add' value
		//  we can do that even if it is below resource (ex: cost = increased heat)
		if( m_costVal > 0 && m_costVal > res ) {
			//not enough of resource to cast spell, so abort
			//todo: send aborted cast because of no resource
			CCLog("CCS: could not pay %f of %s to cast, aborting", m_costVal, m_costStat.c_str() );
			onCooldownStart();
			return;
		}

	}

	double currTime = CastCommandTime::get();
	m_timeStart = currTime;
	
	//spawn effects
	CastTarget* target = m_iOwner->getTarget();
	//target->validateTargets();
	bool hasTargetInRange = target->hasTargetsAtRangeFromEntity(m_pModel->getRange(), m_iOwner);
	if(!hasTargetInRange) {
		CCLOG("CCS: no targets in range on cast of %s", m_pModel->getName().c_str() );
		onCooldownStart();
		return;
	}


	bool foundTarget = false; //ensure we actually reach at least one target



	for( int i=0; i< m_pModel->getNumEffectsOnCast(); i++ )
	{
		//TODO: check for range
		//world->getPhysicsInterface()->GetVecBetween( m_iOwner, 

		

		CastEffect* effect = new CastEffect( );
		effect->init(this, i, m_iOwner, false );

		
		
		//TODO: send all effects as one array so only one "packet" has to travel? //[Optimisation]

		world->addEffectInTransit(m_iOwner, effect, m_iOwner->getTarget(), CastCommandTime::get());

		foundTarget = true;
	}

	if( foundTarget && m_costVal != 0 ) {
		//apply cost
		m_iOwner->incProperty( m_costStat, -1*m_costVal, NULL );
	}

	if( m_pModel->channelTime > 0.0f ) {
		//begin channeling
		m_state = CCS_CHANNELING;
		int numTicks = (m_pModel->channelTime / m_pModel->channelFreq) + 1;
		CastCommandScheduler::get()->scheduleSelector( schedule_selector(CastCommandState::onSchedulerTick), this, m_pModel->channelFreq, numTicks, 0.0f, false);
	}else {
		//CCLog("Cast %s complete!", m_pModel->getName().c_str());
		onCooldownStart();
	}
}

void CastCommandState::onCooldownComplete()
{
	if( m_state != CCS_COOLDOWN ) return;

	//CCLog("Cooldown %s complete!",  m_pModel->getName().c_str());

	double currTime = CastCommandTime::get();
	m_timeStart = currTime;

	m_state = CCS_IDLE;

	//TODO: send cooldown complete signal
}

Json::Value CastCommandState::getDescriptor( std::string dataName )
{
	if( dataName.size() == 0 ) return m_pModel->descriptor;

	return m_pModel->descriptor.get(dataName, Json::Value() );
}