
#ifndef _CASTCOMMANDTIME_H_
#define _CASTCOMMANDTIME_H_


class CastCommandTime
{
protected:
	static double s_time;
public:
	static double get();
	static double updateDelta( float dt );
	static double set( double t );

};

#endif
