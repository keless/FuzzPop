#include "GameEntity.h"

#include "CastWorldModel.h"
#include "CastCommandTime.h"

GameEntity::GameEntity(std::string name, JsonValueRef animLogicJson)
{
	m_animLogic = AnimationLogic::create( animLogicJson );

	m_name = name;
	hp_base = hp_curr = 100;

	mana_base = mana_curr = 100;

	xp_level = 1;
	xp_next = 0;
	xp_curr = 0;

	int_base = int_curr = 10;
	str_base = str_curr = 10;
	agi_base = agi_curr = 10;

	for( int i=0; i< INVENTORY_SIZE; i++)
		m_items[i] = NULL;

	m_statsMap["hp_base"] = &hp_base;
	m_statsMap["hp_curr"] = &hp_curr;
	m_statsMap["mana_base"] = &mana_base;
	m_statsMap["mana_curr"] = &mana_curr;
	m_statsMap["int_base"] = &int_base;
	m_statsMap["int_curr"] = &int_curr;
	m_statsMap["str_base"] = &str_base;
	m_statsMap["str_curr"] = &str_curr;
	m_statsMap["agi_base"] = &agi_base;
	m_statsMap["agi_curr"] = &agi_curr;
	m_statsMap["xp_curr"] = &xp_curr;
	m_statsMap["xp_next"] = &xp_next;
	m_statsMap["xp_level"] = &xp_level;
	m_statsMap["xp_worth"] = &xp_curr;

	m_abilityTargets = new CastTarget();
}

GameEntity::~GameEntity(void)
{
	CCLog("~GameEntity(%s)", m_name.c_str());
	CC_SAFE_RELEASE_NULL( m_abilityTargets );

	for( int i=0; i< m_abilities.size(); i++)
	{
		CC_SAFE_RELEASE(m_abilities[i]);
	}

	for( int i=0; i< INVENTORY_SIZE; i++)
	{
		CC_SAFE_RELEASE(m_items[i]);
	}
}


void GameEntity::doItemEquip( GameItem* item, int idx )
{
	doItemUnequip( idx );

	m_items[idx] = item;
	if( item == NULL ) return;

	item->retain();

	//get abilities, apply to player ability list
	//todo: what if another item provides the same ability? okay to treat as non-unique?
	const Json::Value& abilities = item->getAbilitiesJson();
	for( int abil=0; abil<abilities.size(); abil++){
		CastCommandModel* ccm = new CastCommandModel( abilities[abil] );
		addAbility(ccm);
	}

	//todo: apply non-ability effects? such as stat buffs

}
void GameEntity::doItemUnequip(  int idx )
{

	if( m_items[idx]  == NULL ) return;

	//todo: remove items abilities
	const Json::Value& abilities =  m_items[idx]->getAbilitiesJson();
	for( int abil=0; abil<abilities.size(); abil++){
		std::string abilName = abilities[abil].get("name", "effectName").asString();
		remAbility(abilName);
	}

	//todo: remove non-ability effects? such as stat buffs

	CC_SAFE_RELEASE_NULL(m_items[idx]);
}

void GameEntity::setItemAtSlot( int slotIdx, GameItem* item)
{
	if( slotIdx < 0 || slotIdx >= INVENTORY_SIZE ) return; //oob

	doItemEquip(item, slotIdx);

}

GameItem* GameEntity::getItemAtSlot(int slotIdx )
{
	if( slotIdx < 0 || slotIdx >= INVENTORY_SIZE ) return NULL; //oob

	return m_items[slotIdx];
}

Json::Value GameEntity::toJson()
{
	Json::Value json;
	json["name"] = m_name;
	
	Json::Value stats;
	std::map<std::string, int*>::iterator itr;
	for( itr = m_statsMap.begin(); itr != m_statsMap.end(); itr++)
	{
		stats[ itr->first ] = *(itr->second);
	}
	json["stats"] = stats;

	Json::Value inventory;
	for( int i=0; i< INVENTORY_SIZE; i++)
	{
		if( m_items[i] == NULL ) 
		{
			inventory.append( Json::Value(Json::nullValue));
		}else {
			inventory.append( m_items[i]->toJson() );
		}
	}
	json["inventory"] = inventory;


	return json;
}


void GameEntity::initFromJson( const Json::Value& json )
{
	m_name = json.get("name", "foo").asString();

	if( json.isMember("stats") ) {
		const Json::Value& stats = json["stats"];
		Json::Value::Members statNames = stats.getMemberNames();
		for( int i=0; i< statNames.size(); i++)
		{
			if( m_statsMap.count( statNames[i] ) > 0 ) 
			{
				*(m_statsMap[ statNames[i] ]) = stats[statNames[i]].asInt();
			}
		}
	}

	if( json.isMember("inventory") ) {
		const Json::Value& inventory = json["inventory"];
		for( int i=0; i< INVENTORY_SIZE; i++) {
			if( i >= inventory.size() ) 
			{
				setItemAtSlot(i, NULL);
			}else {

				if( inventory[i].isNull() ) {
					setItemAtSlot(i, NULL);
				}else {
					GameItem* item = new GameItem("");
					item->initFromJson( inventory[i] );
					setItemAtSlot(i, item);



				}
			}
		}
	}else {
		for(int i=0;i< INVENTORY_SIZE; i++) {
			setItemAtSlot(i, NULL);
		}
	}


}

std::string GameEntity::getLevelStr()
{
	char buff[128];
	snprintf(buff, 128, "%d", xp_level);
	return buff;
}

float GameEntity::getDebuffTimeLeft( std::string debufName )
{
	if( m_debuffs.count(debufName) <= 0 ) return 0.0f;

	CastEffect* debuff = m_debuffs[debufName];

	double start = debuff->getStartTime();
    float period =	debuff->getLifeTime();
	double curr = CastCommandTime::get();
	float dt = curr - start;

	return MAX(period - dt, 0.0f);
}

//virtual 
void GameEntity::addAbility( CastCommandModel* ability )
{
	CastCommandState* state = new CastCommandState(ability, this);
	state->retain();
	m_abilities.push_back(state);
}

//virtual 
void GameEntity::remAbility( std::string name )
{
	for( int i=0; i< m_abilities.size(); i++)
	{
		if( m_abilities[i]->getName() == name ) {
			m_abilities[i]->release();
			m_abilities.erase( m_abilities.begin() + i );
			break;
		}
	}
}

std::vector<CastCommandState*>& GameEntity::getAbilityList()
{
	return m_abilities;
}

bool GameEntity::canCast()
{
	for( int i=0; i< m_abilities.size(); i++) {
		if( m_abilities[i]->isCasting() || m_abilities[i]->isChanneling() ) {
			//abort touch
			return false;
		}
	}

	if(! m_abilityTargets->hasValidTarget() ) return false;

	return true;
}

void GameEntity::setProperty( std::string propName, float value, CastEffect* effect )
{
	if( m_statsMap.count(propName) == 0 ) return;
	(*m_statsMap[propName]) = value;

	if( propName.compare("hp_base") == 0 ) {
		if( hp_curr > hp_base ) {
			hp_curr = hp_base; //bound hp_curr to max
		}
	}

	dispatch("incProperty", new GameEntityPropertyChangeEvt(propName, value ));
}

void GameEntity::incProperty( std::string propName, float value, CastEffect* effect )
{
	if( hp_curr == 0 ) {
		if( propName.compare("hp_curr") == 0 && value < 0 ) {
			//dont beat a dead entity
			return;
		}
	}

	if( m_statsMap.count(propName) == 0 ) return;
	(*m_statsMap[propName]) += value;

	//
	if( propName.compare("hp_base") == 0 ) {
		if( hp_curr > hp_base ) {
			hp_curr = hp_base; //bound hp_curr to max
		}
	}
	
	//bounds check hp_curr
	if( propName.compare("hp_curr") == 0 ) {
		if( hp_curr < 0 ) hp_curr = 0;
		if( hp_curr > hp_base ) hp_curr = hp_base;

		if( hp_curr == 0 ) {
			
			GameEntity* killer = NULL;
			if( CastWorldModel::get()->isValid( effect->getOrigin() ) ) {
				killer = dynamic_cast<GameEntity*>( effect->getOrigin() );
			}
			CCLog("%s: I am slain, by %s!", m_name.c_str(), killer?killer->getName().c_str():"a ghost");

			GameEntityDeathEvt* evt = new GameEntityDeathEvt(killer, this);
			EventBus::game()->dispatch("GameEntityDeathEvt", evt);
		}
	}else if( xp_next != 0 &&  propName.compare("xp_curr") == 0  ) {
		if( xp_curr >= xp_next ) {
			CCLOG("player level up " );

			xp_curr -= xp_next;
			//TODO: figure out what the xp_next value should change to
			xp_next *= 2;
			xp_level++;

			GameEntityLevelupEvt* evt = new GameEntityLevelupEvt(this);
			EventBus::game()->dispatch("GameEntityLevelupEvt", evt);


		}
	}

	//CCLog("stat %s delta %0.2f", propName.c_str(), value);

	
	dispatch("incProperty", new GameEntityPropertyChangeEvt(propName, value ));


	//TODO -- if HP <= 0 do logic (set dirty flag and check in update loop)
}

void GameEntity::startBuffProperty( std::string propName, float value, CastEffect* buff)
{
	if( m_statsMap.count(propName) == 0 ) return;
	(*m_statsMap[propName]) += value;
	
	//CCLOG("start buff %s - %f", propName.c_str(), CastCommandTime::get() );

	if( propName.compare("hp_base") == 0 ) {
		if( hp_base < 0 ) hp_base = 0;
		if( hp_curr < hp_base ) hp_curr = hp_base;
	}

}
void GameEntity::endBuffProperty( std::string propName, float value, CastEffect* buff)
{
	if( m_statsMap.count(propName) == 0 ) return;


	(*m_statsMap[propName]) += value;

	//CCLOG("end buff %s - %f", propName.c_str(), CastCommandTime::get() );

	if( propName.compare("hp_base") == 0 ) {

		if( value > 0 ) {
			//returning max health

		}else {
			//losing max health
			if( hp_base < 0 ) hp_base = 0;
			if( hp_curr < hp_base ) hp_curr = hp_base;
		}

		
		
	}
}

float GameEntity::getProperty( std::string propName )
{
	if( m_statsMap.count(propName) == 0 ) return 0;
	return (*m_statsMap[propName]);
}

void GameEntity::handleEffectReaction( Json::Value& reaction, CastEffect* source )
{
	GameEntityReactEvt* evt = new GameEntityReactEvt( reaction, source );
	dispatch("react", evt);
}

void GameEntity::handleEffectEvent( std::string effectEventName, CastEffect* source )
{
	GameEntityEffectEvt* evt = new GameEntityEffectEvt( effectEventName, source->getOrigin(), this );
	EventBus::game()->dispatch("GameEntityEffectEvt", evt);
}

CastTarget* GameEntity::getTarget()
{
	return m_abilityTargets;
}

void GameEntity::applyEffect( CastEffect* effect )
{
	
	if( effect->getLifeTime() == 0 ) 
	{
		//CCLog("apply instant effect");

		effect->doEffect();

	}
	else {
		//CCLog("apply effect over time");

		if( effect->getType() == CET_BUFF_STAT ) 
		{
			m_buffs[effect->getName()] = effect;
		}else if( effect->getType() == CET_SUPPRESS_STAT )
		{
			m_debuffs[effect->getName()] = effect;
		}
		else if( effect->getType() == CET_DAMAGE_STAT )
		{
			m_negativeEffects.push_back(effect);
		}else {
			m_positiveEffects.push_back(effect);
		}

		
		effect->retain();
		effect->startTicks();
	}


 }

void GameEntity::removeEffect( CastEffect* effect )
{
	
	if( effect->getType() == CET_BUFF_STAT ) 
	{
		m_debuffs.erase( effect->getName() );
	}
	else if( effect->getType() == CET_SUPPRESS_STAT )
	{
		m_debuffs.erase( effect->getName() );
	}
	else if( effect->getType() == CET_DAMAGE_STAT )
	{
		for( int i=0; i< m_negativeEffects.size(); i++ )
		{
			if( m_negativeEffects[i] == effect ) {
				m_negativeEffects.erase( m_negativeEffects.begin() + i );
				return;
			}
		}
	}else {
		for( int i=0; i< m_positiveEffects.size(); i++ )
		{
			if( m_positiveEffects[i] == effect ) {
				m_positiveEffects.erase( m_positiveEffects.begin() + i );
				return;
			}
		}
	}

	CC_SAFE_RELEASE( effect );





}
