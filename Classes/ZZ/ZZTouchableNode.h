#ifndef _ZZTOUCHABLE_H_
#define _ZZTOUCHABLE_H_

/**************

TouchableNode is a CCLayer that will fire off a JsonEvent to 
	the EventBus when any of its children are touched.

if 'evt' is "", no event is sent
if 'setKill' is called, when the touch event is fired, the target node will  
	call removeFromParentAndCleanup(true)
if 'StringGrab' is called, when the touch event is fired, the JsonEvent will 
	include a 'string' parameter which holds the value of the CCLabelProtocol

***************/

#include "ZZDefines.h"
#include "ZZEventBus.h"

namespace ZZ {

class TouchableNode : public CCLayer
{
protected:
	std::string m_evt;
	std::string m_bus;
	Json::Value m_data;

	bool m_touchStarted;

	CCNode* m_pKillOnTouch;
	CCLabelProtocol* m_pGrabStringOnTouch;

public:
	TouchableNode( std::string evt, std::string bus = "game");
	virtual ~TouchableNode(void) {}

	void setKill( CCNode* killThis ) { m_pKillOnTouch = killThis; }
	void setStringGrab( CCLabelProtocol* label ) { m_pGrabStringOnTouch = label; }

	void setData( JsonValueRef data) { m_data = data; }
	Json::Value& getData() { return m_data; }

	virtual void onTouched();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

	//virtual void addChild(CCNode *child, int zOrder, int tag);
};

}

#endif