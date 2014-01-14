#include "CastCommandTime.h"


//static 
double CastCommandTime::s_time = 0;
//static 
double  CastCommandTime::get()
{
	return s_time;
}
//static 
double  CastCommandTime::updateDelta( float dt )
{
	s_time += dt;
	return s_time;
}
//static 
double  CastCommandTime::set( double t )
{
	s_time = t;
	return s_time;
}