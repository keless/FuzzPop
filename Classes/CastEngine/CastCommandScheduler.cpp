#include "CastCommandScheduler.h"


//static 
CastCommandScheduler* CastCommandScheduler::s_instance = NULL;

//static
CastCommandScheduler* CastCommandScheduler::get()
{
	if( s_instance == NULL )
	{
		s_instance = new CastCommandScheduler();
	}

	return s_instance;
}

CastCommandScheduler::CastCommandScheduler(void)
{
}


CastCommandScheduler::~CastCommandScheduler(void)
{
}
