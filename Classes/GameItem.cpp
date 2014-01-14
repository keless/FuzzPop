#include "GameItem.h"



GameItem::GameItem(std::string name)
{
	m_name = name;
}


GameItem::~GameItem(void)
{
}

Json::Value GameItem::toJson()
{
	Json::Value json;
	json["name"] = m_name;
	json["type"] = itemTypeToString( m_type );
	json["abilities"] = m_abilities;

	return json;
}

void GameItem::initFromJson( const Json::Value& json )
{
	m_name = json.get("name", "item").asString();

	m_type = itemStringToType( json.get("type", "item").asString() );

	m_abilities = json.get("abilities", Json::Value() );

}


//static 
std::string GameItem::itemTypeToString( GameItemType type )
{
	switch(type)
	{
		case GIT_ARMOR:
			return "armor";
		case GIT_EQUIPMENT:
			return "equipment";
		case GIT_WEAPON:
			return "weapon";
	}
	return "item";
}

//static 
GameItemType GameItem::itemStringToType( const std::string& type )
{
	if( type == "armor" ) return GIT_ARMOR;
	if( type == "weapon" ) return GIT_WEAPON;
	return GIT_EQUIPMENT;
}


