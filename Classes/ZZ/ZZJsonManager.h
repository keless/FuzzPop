#ifndef _ZZ_JSONMANAGER_H_
#define _ZZ_JSONMANAGER_H_

#include "ZZDefines.h"

namespace ZZ {

class JsonManager
{
protected:
	static JsonManager* s_instance;
public:
	static JsonManager* get();

protected:
	Json::Value m_domains;


	JsonManager(void);
	~JsonManager(void);
public:

	//note: allows overwrite of domains
	void initFromFile( const std::string& fileName );

	//note: allows overwrite of domains
	void addJson( const std::string& domain, const JsonValueRef json );

	Json::Value getJson( const std::string& domain, const std::string& path = "");

};

}

#endif