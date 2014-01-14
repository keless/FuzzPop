#ifndef _ZZTEXTFIELD_H_
#define _ZZTEXTFIELD_H_

#include <cocos2d.h>
using namespace cocos2d;

#include "ZZEventBus.h"

/*************
wraps the CCTextFieldTTF class in a more easy to use ZZ-friendly class

if you setEventBus, it will send signals "zztfStart" and "zztfEnd" to it on IME begin and end
 the JsonEvent will have a pUserData pointing at the TextField instance that spawned the event

**************/

namespace ZZ {

class TextField : public CCLayer, public CCLabelProtocol
{
protected:
	CCTextFieldTTF* m_tf;

	CCDrawNode* m_outline;

	std::string m_defaultText;
	bool m_touchStarted;
	bool m_inEditMode;

	std::string m_evtBus;

public:
	TextField(void);
	virtual ~TextField(void);

	static TextField* create( std::string defaultText = "", std::string fontName = "Arial", int size = 24, const CCSize& dimensions = CCSizeMake(200,60), CCTextAlignment alignment = kCCTextAlignmentLeft );

	virtual bool init( std::string defaultText, std::string fontName, int size, const CCSize& dimensions, CCTextAlignment alignment  = kCCTextAlignmentLeft );

	virtual void setEventBus( std::string bus );

	virtual void setString(const char *label);
    virtual const char* getString(void);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
};

}

#endif