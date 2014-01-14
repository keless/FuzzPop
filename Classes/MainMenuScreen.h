#ifndef __MAINMENUSCREEN_H__
#define __MAINMENUSCREEN_H__

#include "ZZUtils.h"
using namespace ZZ;

class MainMenuScreen : public CCLayer
{
public:
	MainMenuScreen(void);
	~MainMenuScreen(void);
    CREATE_FUNC(MainMenuScreen);
	bool init();

    // a selector callback
    void menuStartCallback(CCObject* pSender);
	void menuAnimViewerCallback(CCObject* pSender);

};

#endif

