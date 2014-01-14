#ifndef _ANIMATION_SCREEN_H_
#define _ANIMATION_SCREEN_H_

#include "ZZUtils.h"
using namespace ZZ;

class AnimationScreen : public CCLayer
{
protected:

	AnimatedSprite* m_pony;


public:
	AnimationScreen(void);
	~AnimationScreen(void);

	CREATE_FUNC(AnimationScreen);
	bool init();

	void eventMenuCallback(CCObject* pSender);
};

#endif