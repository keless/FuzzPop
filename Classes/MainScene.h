#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "ZZUtils.h"
using namespace ZZ;

class MainScene : public cocos2d::CCLayer
{

	CCLayer* m_activeLayer;
	MainScene() : m_activeLayer(NULL) { }

public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

	virtual void update( float dt );


    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
	void doStateChange( CCObject* e ); 

    // a selector callback
    void menuCloseCallback(CCObject* pSender);

    virtual void ccTouchesBegan(CCSet* touches, CCEvent* event);
    virtual void ccTouchesMoved(CCSet* touches, CCEvent* event);
    virtual void ccTouchesEnded(CCSet* touches, CCEvent* event);
    virtual void ccTouchesCancelled(CCSet* touches, CCEvent* event);
    
    // implement the "static node()" method manually
    CREATE_FUNC(MainScene);
};

#endif // __HELLOWORLD_SCENE_H__
