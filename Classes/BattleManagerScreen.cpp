#include "BattleManagerScreen.h"

#define MAX_STEP 1.0f
#define TIME_SCALE 0.5f
#include "CastCommandTime.h"
#include "CastCommandScheduler.h"
#include "CastWorldModel.h"

#define GAME_UNIT_CONVERSION (1.0f/260.0f)
#define VIEW_UNIT_CONVERSION (260.0f)

//static 
BattleManagerScreen* BattleManagerScreen::create()
{
	BattleManagerScreen* bm = new BattleManagerScreen();
	bm->init();
	bm->autorelease();
	return bm;
}

BattleManagerScreen::BattleManagerScreen(void)
{
	m_travelProgess = 0;
	m_pbTravel = NULL;
}

bool BattleManagerScreen::init()
{

	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    //CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	setContentSize( visibleSize );
	
	initAbilities();

	m_travelLastSpawnLocation = 0.0f;
	m_travelProgess = 0.0f;
	m_travelDistance = 100.0f;
	float pbMargin = 50;
	m_pbTravel = ProgressBar::create( CCRectMake( pbMargin, visibleSize.height - pbMargin, visibleSize.width - pbMargin*2, pbMargin ) );
	m_pbTravel->setProgress( m_travelProgess / m_travelDistance );
	addChild(m_pbTravel);

	//todo: remove listener on destructor
	EventBus::game()->addListener("GameEntityDeathEvt", this, callfuncO_selector(BattleManagerScreen::onEntityDeath));
	EventBus::game()->addListener("GameEntityLevelupEvt", this, callfuncO_selector(BattleManagerScreen::onEntityLevelup));
	EventBus::game()->addListener("GameEntityEffectEvt", this, callfuncO_selector(BattleManagerScreen::onEntityEffectEvent));
	
	Json::Value data = ReadFileToJson("ponySprite.json");
	m_ponyAnimLogic = AnimationLogic::create(data["animLogic"]);
	m_ponyControllerModel = new EntityAnimController(m_ponyAnimLogic);
	m_ponyControllerModel->setImpulseMapping("walk", "walk");
	m_ponyControllerModel->setImpulseMapping("meleeAttack", "attack1");
	m_ponyControllerModel->setImpulseMapping("castAttack", "attack2");
	m_ponyControllerModel->setImpulseMapping("rangedAttack", "attack3");
	m_ponyControllerModel->setImpulseMapping("startJump", "jump");
	m_ponyControllerModel->setImpulseMapping("doubleJump", "jump");
	m_ponyControllerModel->setImpulseMapping("startDeath", "die");

	CastWorldModel::get()->setPhysicsInterface(this);

	initPartyFromJson();


	spawnEnemy();

	scheduleUpdate();
	//setTouchEnabled(true);
    
    return true;
}

BattleManagerScreen::~BattleManagerScreen(void)
{
	EventBus::game()->remListener("GameEntityDeathEvt", this, callfuncO_selector(BattleManagerScreen::onEntityDeath));
	EventBus::game()->remListener("GameEntityLevelupEvt", this, callfuncO_selector(BattleManagerScreen::onEntityLevelup));
	EventBus::game()->remListener("GameEntityEffectEvt", this, callfuncO_selector(BattleManagerScreen::onEntityEffectEvent));
}

float BattleManagerScreen::getPartySpeed()
{
	return 15.0f; //todo: derive from party stats
}

void BattleManagerScreen::removeEntity( GameEntity* entity, bool isEnemy )
{
	
	//CCLog("begin remove entity at 0x%d", entity);

	if( isEnemy ) {
		bool removed = false;
		for( int i=m_enemies.size()-1; i >= 0; i--) {
			EntityPair enemy = m_enemies[i];
			//CCLog("compare 0x%d to 0x%d", enemy.model, entity);
			if( (enemy.model) == entity ) {
				m_enemies.erase( m_enemies.begin() + i );
				//CCLog("removed enemy %d 0x%d", i, enemy.model);
				removed = true;
				break;
			}
		}
		if(!removed) {
			CCLog("wtf");
		}
	}else {
		bool removed = false;
		for( int i=m_players.size()-1; i >= 0; i--) {
			EntityPair enemy = m_players[i];
			if( (enemy.model) == entity ) {
				m_players.erase( m_players.begin() + i );
				removed = true;

				break;
			}
		}
		if(!removed) {
			CCLog("wtf");
		}
	}

	for( int i=m_allEntities.size()-1; i >= 0; i--) {
		EntityPair enemy = m_allEntities[i];
		if( (enemy.model) == entity ) {
			m_allEntities.erase( m_allEntities.begin() + i );
			

			//CCLog("removed entity %d 0x%d", i, enemy.model);
			enemy.view->detatchFromEntity();
			CC_SAFE_RELEASE_NULL(enemy.model);
			CC_SAFE_RELEASE_NULL(enemy.view);

			break;
		}
	}

	//CCLOG("%d entities %d enemies left", m_allEntities.size(), m_enemies.size() );

}

//virtual 
void BattleManagerScreen::update( float dt )
{
	if( dt > MAX_STEP ) dt = MAX_STEP;

	dt *= TIME_SCALE;

	CastCommandTime::updateDelta(dt);
	CastCommandScheduler::get()->update(dt);
	CastWorldModel::get()->updateStep(dt);

	if( m_players.size() < 1 ) 
	{
		//todo: handle game over 
		BaseEvent* evt = new BaseEvent("mainMenu");
		EventBus::get("state")->dispatch("switchTo", evt);
		return;
	}

	for( int i=m_enemies.size()-1; i >= 0; i--) {
		EntityPair& enemy = m_enemies[i];

		if( enemy.model->getProperty("hp_curr") <= 0 ) {
			setCardDeath(enemy.view);
			//removeChild(enemy.enemyView);

			removeEntity( enemy.model, true );
		}
		else {
			//enemyMovementAI(i, dt);  //put movement logic into PerformEnemyAI to synch with animations

			PerformEnemyAI(dt, &enemy);
		}
	}


	if( m_enemies.size()  == 0 ) {


		//move party forward

		float speed = getPartySpeed();
		m_travelProgess += speed * dt;

		//TODO: set party animations to walk

		m_pbTravel->setProgress( m_travelProgess/ m_travelDistance );

		if( m_travelProgess >= m_travelLastSpawnLocation + 25 )
		{
			//spawn more
			m_travelLastSpawnLocation = m_travelProgess +  25;
			

			int randNum = 1 + (rand() % 3);
			CCLog("spawn %d enemies", randNum);
			for( int i=0; i< randNum; i++) {
				spawnEnemy();
			}
		

		}

		if( m_travelProgess >= m_travelDistance )
		{
			//reached target, do something
			EventBus::get("state")->dispatch("switchTo", new BaseEvent("mainMenu") );
			return;
		}
	}
	
	
	for( int i=0; i< m_players.size(); i++)
	{
		EntityPair& player = m_players[i];
		if( player.model->getProperty("hp_curr") <= 0 ) {
			setCardDeath(player.view);
			//removeChild(player.enemyView);

			removeEntity( player.model, false );
		}else {
			PerformPlayerAi(dt, &(m_players[i]) );
		}

		
	}

	
}


//#define DISABLE_ATTACKS

void BattleManagerScreen::PerformEnemyAI( float dt, EntityPair* enemy )
{
	if( !CastWorldModel::get()->isValid( enemy->model ) ) return; //skip dead/dying entities

	//skip if busy (should be 'idle' or 'walk' if available)
	std::string currState = enemy->view->getCurrAnimName();
	if( currState != "walk" && currState != "idle") return; //entity is busy

	//select ability
	std::vector<CastCommandState*> abilities;
	abilities = enemy->model->getAbilityList();

	CastCommandState* cast = abilities[ rand() % abilities.size() ];

	//TODO: if beneficial, select friendly target
	float range = 0;
	EntityPair* player = getClosestPlayerToEnemy(enemy, range);

	if( player == NULL ) {
		//no target, do nothing
		return;
	}

		//select target
	enemy->model->getTarget()->clearTargetEntities();
	enemy->model->getTarget()->addTargetEntity(player->model);

	//todo: check attack animation type requirement
	// meleeAttack, castAttack, rangedAttack -- based on weapon type?
	if( enemy->model->canCast() && (cast->getRange() >= range)
		&& enemy->model->handleEntityCommand("meleeAttack", enemy->view, true)
		) {

#ifndef DISABLE_ATTACKS
			CCLOG("EnemyAI: perform attack");
		cast->startCast();
#endif

	}else {
		//see if we should walk towards player
		enemyMovementAI(dt, enemy, player);
	}
}

EntityPair* BattleManagerScreen::getClosestPlayerToEnemy( EntityPair* enemy, float& outRange )
{
	//handle selection from multiple players (heuristic: shortest distance)
	float shortestDistanceSQ = 999999;
	int idx = -1;
	for(int i=0; i< m_players.size(); i++)
	{
		kmVec2 dV;
		GetVecBetween(enemy->model, m_players[0].model, dV);

		float dx = kmVec2LengthSq(&dV);
		if( dx < shortestDistanceSQ )
		{
			shortestDistanceSQ = dx;
			idx = i;
		}
	}

	if( idx < 0 ) return NULL; //no targets within max dist

	outRange = sqrtf(shortestDistanceSQ);

	return &(m_players[idx]);
}

void BattleManagerScreen::enemyMovementAI( float dt, EntityPair* enemy, EntityPair* targetPlayer )
{
	float speed = 75.0f; //5 pixels/sec
	
	std::vector<kmVec2> impulses; //x, y
	std::vector<float> impulseWeights;

	//target was already selected from closest player
	EntityPair* player = targetPlayer;
	
	CCSize eSize = enemy->view->getContentSize();
	CCPoint ePos = enemy->view->getPosition();
	ePos.x += eSize.width/2;
	ePos.y += eSize.height/2; //convert to center origin
	CCSize pSize = player->view->getContentSize();
	CCPoint pPos = player->view->getPosition();
	pPos.x += pSize.width/2;
	pPos.y += pSize.height/2; //convert to center origin
	
	float playerLeash = pSize.width * 1.1f;
	float playerLeashSq = playerLeash*playerLeash;

	//impulse towards the player
	kmVec2 toPlayer = { pPos.x - ePos.x, pPos.y - ePos.y };
	kmVec2 u_toPlayer;
	kmVec2Normalize( &u_toPlayer, &toPlayer);
	
	
	if( kmVec2LengthSq( &toPlayer ) < (playerLeashSq * 0.75f) )
	{
		//impulse away from player (too close)
		kmVec2 u_fromPlayer;
		kmVec2Scale(&u_fromPlayer, &u_toPlayer, -1*0.75f); //flip the 'to player' vector
		
		impulses.push_back(u_fromPlayer);
		impulseWeights.push_back(100); //vastly overweigh the impulse to the player
	}else {

		if( kmVec2LengthSq(&toPlayer) > playerLeashSq )
		{
			//kmVec2Scale(&u_toPlayer, &u_toPlayer, 0.51f);
			//impulse towards player
			impulses.push_back(u_toPlayer);
			impulseWeights.push_back(100);

		}
	}
	
	//add impulses away from other enemies
	for( int i=0; i< m_enemies.size(); i++) {
		if( m_enemies[i].model == enemy->model ) continue; //dont impulse away from self
		
		CCSize nSize = m_enemies[i].view->getContentSize();
		CCPoint nPos = m_enemies[i].view->getPosition();
		nPos.x += nSize.width/2;
		nPos.y += nSize.height/2;
		
		kmVec2 toNeighbor = { nPos.x - ePos.x, nPos.y - ePos.y };

		float neighborLeash = nSize.width;
		if( kmVec2LengthSq(&toNeighbor) < neighborLeash * neighborLeash ) {
			kmVec2 u_toNeighbor;
			kmVec2Normalize(&u_toNeighbor, &toNeighbor);
			kmVec2 u_fromNeighbor;
			kmVec2Scale(&u_fromNeighbor, &u_toNeighbor, -1);
			impulses.push_back(u_fromNeighbor);
			impulseWeights.push_back(50);
		}
	}
	
	if( impulses.size() == 0 ) {
		//set animation to idle/stopped
		enemy->model->handleEntityCommand("idle", enemy->view, true);
		return;
	}

	//blend impulses
	kmVec2 finalImpulse = impulses[0]; //zero always valid because its the impulse to the player
	float finalImpulseWeight = impulseWeights[0];
	for( int i=1; i< impulses.size(); i++) {

		float w1 = finalImpulseWeight;
		float w2 = impulseWeights[i];
		float wTot =  w1 + w2;
		
		kmVec2 blend;
		blend.x = (finalImpulse.x * w1 / wTot) + (impulses[i].x * w2 / wTot);
		blend.y = (finalImpulse.y * w1 / wTot) + (impulses[i].y * w2 / wTot);
		
		//run-length summation
		finalImpulseWeight = wTot;
		finalImpulse = blend;
	}
	
	kmVec2 scaledImpulse;
	kmVec2Scale(&scaledImpulse, &finalImpulse, speed * dt);
	
	//CCLog("impulse %.4f", kmVec2Length(& finalImpulse));
	if( kmVec2Length(& finalImpulse) <  (0.5f) )
	{
		//set animation to idle/stopped
		enemy->model->handleEntityCommand("idle", enemy->view, true);

		//CCLog("ignore impulse %.4f", kmVec2Length(& finalImpulse));
		return; //ignore very small changes to avoid leash jitter
	}

	//TODO: handle walk/run differences?
	if( enemy->model->handleEntityCommand("walk", enemy->view, true) ) 
	{
		ePos.x += scaledImpulse.x;
		ePos.y += scaledImpulse.y;
		ePos.x -= eSize.width/2;
		ePos.y -= eSize.height/2; //back to original anchor coords
		enemy->view->setPosition(ePos);
	}
	

}

void BattleManagerScreen::PerformPlayerAi( float dt, EntityPair* player )
{
	//select ability
	std::vector<CastCommandState*> abilities;
	abilities = player->model->getAbilityList();

	if( abilities.size() == 0 ) return; //no abilities, nothing to do

	CastCommandState* cast = abilities[ rand() % abilities.size() ];

	ICastEntity* target = NULL;
	if( player->model->getTarget()->getEntityList().size() > 0 )
		target = player->model->getTarget()->getEntityList()[0];
	if( !CastWorldModel::get()->isValid( target ) )
	{
		player->model->getTarget()->clearTargetEntities();

		if( m_enemies.size() > 0 ) {

			//target closest enemy
			target = m_enemies[0].model;
			kmVec2 distVec;
			GetVecBetween(player->model, target, distVec);
			float closestEnemy = kmVec2Length( &distVec );

			for( int i=1; i< m_enemies.size(); i++)
			{
				GetVecBetween(player->model, m_enemies[i].model, distVec);
				float distTo = kmVec2Length( &distVec );

				if( distTo < closestEnemy ) {
					closestEnemy = distTo;
					target = m_enemies[i].model;
				}
				
			}

			
			player->model->getTarget()->addTargetEntity(target);
		}
	}

	kmVec2 toTarget;
	if( GetVecBetween(player->model, target, toTarget) && player->model->canCast() && cast->canAfford() ) {
		
		float dTargetSq = kmVec2LengthSq( &toTarget );
		float range = cast->getRange();

		float meleeRange = 1.0f;

		if( range*range >= dTargetSq ) {
		
			bool dontCast = false;
			if( cast->getName().compare("Death Grip") == 0 )
			{
				//abort if in melee range already
				if( dTargetSq <= meleeRange*meleeRange ) {
					dontCast = true;
					//CCLog("target in melee range, abort death grip");
				}
			}
			else if( cast->getName().compare("Curse of Weakness") == 0 ) {
				//abort if already has curse of weakness and time left greater than 2s
				GameEntity* geTarget = dynamic_cast<GameEntity*>(target);
				if( geTarget != NULL ) {
					float lt = geTarget->getDebuffTimeLeft("Curse of Weakness");
					if( lt > 2 ) 
						dontCast = true;
				}
			}

#ifndef DISABLE_ATTACKS
			if( !dontCast ) {
				cast->startCast();
			}
#endif
		}

	}


}

void BattleManagerScreen::onEntityEffectEvent( CCObject* e )
{
	GameEntityEffectEvt* evt = dynamic_cast<GameEntityEffectEvt*>(e);
	if(!evt) return;

	if( evt->name.compare("Death Grip") == 0 ) {
		CCLog("death grip");

		kmVec2 pOrigin;
		kmVec2 pTarget;
		if( !GetEntityPosition(evt->origin, pOrigin) ||  !GetEntityPosition(evt->target, pTarget) )
		{
			CCLog("aborting death grip evt due to invalid target(s)");
		}
		
		kmVec2 toTarget;
		kmVec2Subtract(&toTarget, &pTarget, &pOrigin);

		kmVec2 u_toTarget;
		kmVec2Normalize(&u_toTarget, &toTarget);

		float leashDistance = 1;  //one game unit

		kmVec2 dv;
		kmVec2Scale(&dv, &u_toTarget, leashDistance );

		kmVec2 posEnd;
		kmVec2Add(&posEnd, &pOrigin, &dv);

		//convert back to screen coordinates
		kmVec2Scale(&posEnd, &posEnd, VIEW_UNIT_CONVERSION);

		GameEntityView* tView =	getViewForEntity(evt->target);
		tView->setPosition(ccp(posEnd.x, posEnd.y));
	}

}

void BattleManagerScreen::onEntityDeath( CCObject* e )
{
	GameEntityDeathEvt* evt = dynamic_cast<GameEntityDeathEvt*>(e);
	if(!evt) return;

	bool killerIsPlayer = false;
	for( int i=0; i< m_players.size(); i++)
	{
		if( m_players[i].model == evt->killer ) 
		{
			killerIsPlayer = true;
			break;
		}
	}

	if( killerIsPlayer ) {
		CCLOG("reward XP");

		float rawXP = evt->killed->getProperty("xp_curr");
		int numPlayers = m_players.size();

		int xpPer = ceilf( rawXP / numPlayers );

		for( int i=0; i< m_players.size(); i++)
		{
			m_players[i].model->incProperty("xp_curr", xpPer, NULL);
		}

	}

	

}

void BattleManagerScreen::onEntityLevelup( CCObject* e )
{
	GameEntityLevelupEvt* evt = dynamic_cast<GameEntityLevelupEvt*>(e);
	if(!evt) return;

	GameEntity* entity = dynamic_cast<GameEntity*>( evt->target );

	if( !CastWorldModel::get()->isValid( entity )  ) return;

	//award full health/mana
	entity->setProperty("hp_curr", entity->getProperty("hp_base"), NULL );
	entity->setProperty("mana_curr", entity->getProperty("mana_base"), NULL );
	
}

void BattleManagerScreen::setCardDeath( GameEntityView* view )
{
	CCSequence* seq = CCSequence::create(
		CCScaleTo::create(1.f, 1.2f, 0.1f),
		CCCallFunc::create(view, callfunc_selector( GameEntityView::removeFromParent ) ),
		NULL
		);

	view->runAction( seq );
}


void BattleManagerScreen::initAbilities()
{

	//todo: load abilities from file


	Json::Value json = JsonManager::get()->getJson("EnemyAbilities");
		//ReadFileToJson( "abilities.json" );

	if( ! json.isArray() ) {
		return;
	}

	CastCommandModel* mod = NULL;
	for( int i=0; i< json.size(); i++)
	{
		Json::Value& obj = json[i];

		if( obj.isNull() || ! obj.isObject() ) continue;

		CCLog("load %s",  obj.get("name", "").asString().c_str() );

		mod = new CastCommandModel( obj );
		mod->retain();
		m_abilities[mod->getName()] = mod;
	}
}

void BattleManagerScreen::initPartyFromJson()
{
	Json::Value data = ReadFileToJson("ponySprite.json");

	EntityPair player;
	player.model = new GameEntity("derpy", m_ponyAnimLogic);
	player.model->setAnimController(m_ponyControllerModel);
	//player.model->initFromJson();
	player.view = new GameEntityView( player.model, data["animSprite"] );
	player.view->setPositionX(150);
	player.view->setPositionY(150);
	player.view->setAnimState("idle");
	addChild(player.view);

	
	m_players.push_back(player);
	m_allEntities.push_back(player);

	/*
	Json::Value party = ReadFileToJson( FILE_PARTY_MEMBERS_JSON );

	if( !party.isArray() ) return; //invalid json or empty party

	for( int i=0; i< party.size(); i++)
	{
		EntityPair player;

		const Json::Value& partyMember = party[i];

		player.model = new GameEntity("");
		player.model->initFromJson( partyMember );
		//m_playerModel->incProperty("hp_curr", -90);
		player.model->incProperty("xp_next", 100, NULL);
		//player.model->addAbility( m_abilities["fireball"] );
		//player.model->addAbility( m_abilities["Life Drain"] );
		//player.model->addAbility( m_abilities["Curse of Weakness"] );

		player.view = new GameEntityView( player.model );
		player.view->setPositionX(150);
		player.view->setPositionY(150 + 200 * i);
		addChild(player.view);

		m_players.push_back(player);
		m_allEntities.push_back(player);
	}
	*/
}

bool BattleManagerScreen::GetVecBetween( ICastEntity* from, ICastEntity* to, kmVec2& distVec )
{
	CastWorldModel* world = CastWorldModel::get();

	if( !world->isValid(from) || !world->isValid(to) ) return false;

	kmVec2 pFrom;
	pFrom.x = pFrom.y = 0;
	GameEntityView* fromView = getViewForEntity(from);
	if( fromView != NULL ) 
	{
		pFrom.x = fromView->getPositionX();
		pFrom.y = fromView->getPositionY();
	}
	
	kmVec2 pTo;
	pTo.x = pTo.y = 0;
	GameEntityView* toView = getViewForEntity(to);
	if( toView != NULL ) 
	{
		pTo.x = toView->getPositionX();
		pTo.y = toView->getPositionY();
	}else {
		CCLog("uhoh..");
	}

	kmVec2Subtract( &distVec, &pFrom, &pTo );
	kmVec2Scale(&distVec, &distVec, GAME_UNIT_CONVERSION ); //safe to operate on same vector

	return true;
}

GameEntityView* BattleManagerScreen::getViewForEntity( ICastEntity* entity )
{

	for( int i=0; i< m_allEntities.size() ; i++ )
	{
		if( entity == m_allEntities[i].model )
		{
			return m_allEntities[i].view;
		}
	}

	return NULL;
}

bool BattleManagerScreen::GetEntityPosition( ICastEntity* entity, kmVec2& pos )
{
	GameEntityView* view = getViewForEntity( entity );

	if( view != NULL ) {
		pos.x = view->getPositionX();
		pos.y = view->getPositionY();
		kmVec2Scale(&pos, &pos, GAME_UNIT_CONVERSION);
		return true;
	}

	return false;
}

bool BattleManagerScreen::GetEntitiesInRadius( kmVec2 p, float r, std::vector<ICastEntity*>& entities )
{

	bool found = false;

	float upscale = VIEW_UNIT_CONVERSION;
	kmVec2Scale( &p, &p, upscale );
	r *= VIEW_UNIT_CONVERSION; //convert to pixels

	float rSq = r*r;
	for( int i=0; i< m_allEntities.size(); i++)
	{
		kmVec2 ePos;
		ePos.x = m_allEntities[i].view->getPositionX();
		ePos.y = m_allEntities[i].view->getPositionY();

		float distSq = 0;
		if( ePos.x == p.x ) //handle simple cases first
		{
			distSq = ePos.y - p.y;
			distSq *= distSq;
		}else if( ePos.y == p.y )
		{
			distSq = ePos.x - p.x;
			distSq *= distSq;
		}else {
			kmVec2 dist;
			kmVec2Subtract( &dist, &p, &ePos );
			distSq = kmVec2LengthSq(&dist);
		}

		CCLog("ent %d in radius dist %f", i, sqrt(distSq));

		if( distSq  <= rSq ) {
			entities.push_back( m_allEntities[i].model );
			found = true;
		}
	}

	return found;
}


void BattleManagerScreen::spawnEnemy()
{
	CCSize screen = boundingBox().size;

	Json::Value data = ReadFileToJson("ponySprite.json");

	EntityPair enemy;
	enemy.model =  new GameEntity("Giant Rat", data["animLogic"]);
	enemy.model->addAbility( m_abilities["Bite"] );
	enemy.model->setAnimController(m_ponyControllerModel);  //todo: use approprite controllers for various entity types
	//enemy.enemyModel->incProperty("hp_curr", -90);
	enemy.model->setProperty("hp_base", 50, NULL);
	enemy.model->setProperty("xp_curr", 10, NULL);
	enemy.view =  new GameEntityView( enemy.model, data["animSprite"] );
	enemy.view->setFlipX(true);
	enemy.view->setAnimState("idle");
	//enemy.view->setBackground("rat.png");
	
	int offY = (rand()%100) - 50;
	enemy.view->setPosition( ccp(screen.width, 220 + offY)	);
	addChild(enemy.view);
	m_enemies.push_back(enemy);

	m_allEntities.push_back(enemy);
}

