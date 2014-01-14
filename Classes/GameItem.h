#ifndef _H_GAMEITEM_
#define _H_GAMEITEM_

#include "ZZUtils.h"
using namespace ZZ;

enum GameItemType
{
	GIT_ARMOR = 0,
	GIT_EQUIPMENT,
	GIT_WEAPON,

	GIT_COUNT
};

class GameItem : public CCObject
{
	GameItemType m_type;
	std::string m_name;

	Json::Value m_abilities;

public:
	GameItem(std::string name);
	~GameItem(void);

	Json::Value toJson();
	void initFromJson( const Json::Value& json );
	
	std::string getName() { return m_name; }
	void setName(std::string name) { m_name = name; }

	GameItemType getType() { return m_type; }

	const Json::Value& getAbilitiesJson() { return m_abilities; }

	static std::string itemTypeToString( GameItemType type );
	static GameItemType itemStringToType( const std::string& type );
};


#endif