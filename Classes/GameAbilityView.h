#ifndef _GAMEABILITYVIEW_H_
#define _GAMEABILITYVIEW_H_


#include "cocos2d.h"
using namespace cocos2d;

#include "GameEntity.h"
#include "ZZProgressBar.h"

#include "json.h"

class GameAbilityView :
	public CCNode
{
	CastCommandState* m_pAbility;

	CCLabelTTF*		m_lblName;
	CCLayerColor*	m_cdOverlay;
	CCLayerColor*	m_castOverlay;

public:
	GameAbilityView( CastCommandState* ability );
	~GameAbilityView(void);

	bool canCast();
	bool startCast();

	virtual void update( float delta );
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

#endif
