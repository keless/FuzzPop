#include "ZZJsonManager.h"

#include "ZZUtils.h"

namespace ZZ {
//static 
JsonManager* JsonManager::s_instance = NULL;

//static 
JsonManager* JsonManager::get()
{
	if( s_instance == NULL ) 
	{
		s_instance = new JsonManager();
	}
	return s_instance;
}

JsonManager::JsonManager()
{
}


JsonManager::~JsonManager(void)
{
}

void JsonManager::initFromFile( const std::string& fileName )
{
	Json::Value json = ReadFileToJson(fileName);
	if( json.isObject() ) {
		Json::Value::Members domains = json.getMemberNames();
		for(int i=0; i< domains.size(); i++){
			m_domains[ domains[i] ] = json[domains[i]];
			CCLog("loaded json domain %s", domains[i].c_str() );
		}
	}
}

void JsonManager::addJson( const std::string& domain, const JsonValueRef json )
{
	m_domains[domain] = json;
}

Json::Value JsonManager::getJson( const std::string& domain, const std::string& path)
{
	Json::Value val;

	if( m_domains.isMember(domain) )  {
		if( path.size() == 0 ) {
			val = m_domains[domain];
		}else {
			//todo: do recursive path splitting
			val = m_domains[domain].get(path, val);
		}
	}

	return val;
}

}