#include "ZZEventBus.h"

namespace ZZ {

//static 
EventBus* EventBus::s_gameBus = NULL;
//static 
std::map<std::string, EventBus*> EventBus::s_busHash;

//static 
EventBus* EventBus::game()
{
	if( s_gameBus == NULL ) {
		s_gameBus = new EventBus();
	}
	return s_gameBus;
}

//static 
EventBus* EventBus::get( const char* busName )
{
	if( s_busHash.count(busName) < 1 ) 
	{
		if( strcmp("game", busName) == 0 ) {
			s_busHash["game"] = game();
		}else {
			s_busHash[busName] = new EventBus();
		}
	}

	return s_busHash[busName];
}

EventBus::EventBus(void)
{
}


EventBus::~EventBus(void)
{
}

void EventBus::addListener(std::string evtName, CCObject* listener, SEL_CallFuncO callback  )
{
	callbackPair cbp;
	cbp.callback = callback;
	cbp.listener = listener;

	if( m_listeners.count( evtName ) == 0 ) {
		m_listeners[evtName] = std::vector<callbackPair>();
	}

	m_listeners[evtName].push_back(cbp);
}

void EventBus::remListener(std::string evtName, CCObject* listener, SEL_CallFuncO callback  )
{
	if( m_listeners.count( evtName ) == 0 ) {
		return;
	}

	std::vector<callbackPair> & listeners = m_listeners[evtName];
	for( int i=0; i< (int) listeners.size(); i++) 
	{
		if( listeners[i].callback == callback && listeners[i].listener == listener )
		{
			listeners.erase( listeners.begin() + i );
			return;
		}
	}
}

void EventBus::dispatch(std::string evtName, CCObject* evtObject )
{
	std::vector<callbackPair> & listeners = m_listeners[evtName];
	for( int i=0; i< (int) listeners.size(); i++) 
	{
		
		CCCallFuncO* cfunc = CCCallFuncO::create(listeners[i].listener, listeners[i].callback, evtObject);
		cfunc->execute();
		CC_SAFE_RELEASE_NULL(cfunc);

	}

	//CCLog("dispatch %s complete", evtName.c_str());
}

void EventBus::dispatch( BaseEvent* evt )
{
	dispatch(evt->type, evt);
}

}

