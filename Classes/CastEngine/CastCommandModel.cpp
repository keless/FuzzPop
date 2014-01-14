#include "CastCommandModel.h"


CastCommandModel::CastCommandModel(const Json::Value& castData)
{
	descriptor = castData;

	name = descriptor.get("name", "effectName").asString();
	castTime = descriptor.get("castTime", 0.0f).asDouble();
	channelTime = descriptor.get("channelTime", 0.0f).asDouble();
	channelFreq = descriptor.get("channelFreq", 1.0f).asDouble();
	travelSpeed = descriptor.get("travelSpeed", 0.0f).asDouble();
	range = descriptor.get("range", 0.0f).asDouble();
	effectWhileTraveling = descriptor.get("effectWhileTravel", false).asBool();
	stopOnHit = descriptor.get("stopOnHit", true).asBool();
	effectSize = descriptor.get("effectSize", 0.0f).asDouble();
	cooldownTime = descriptor.get("cooldownTime", 0.0f).asDouble();

	if( ! descriptor.isMember("effectsOnCast") || !descriptor["effectsOnCast"].isArray() ) 
	{
		if(! descriptor.isMember("effectsOnChannel") ) {
			CCLOGERROR("CastCommandModel %s is missing effectsOnCast array", name.c_str());
		}
	}
	
	//effectsOnCast = castData.get("effectsOnCast", Json::Value());
}


CastCommandModel::~CastCommandModel(void)
{
}

int CastCommandModel::getNumEffectsOnChannel()
{
	return descriptor["effectsOnChannel"].size();
}
Json::Value CastCommandModel::getEffectOnChannel( int idx )
{
	return descriptor["effectsOnChannel"][idx];
}

int CastCommandModel::getNumEffectsOnCast()
{
	return descriptor["effectsOnCast"].size();
}
Json::Value CastCommandModel::getEffectOnCast( int idx )
{
	return descriptor["effectsOnCast"][idx];
}