#ifndef _GAMEITEMVIEW_H_
#define _GAMEITEMVIEW_H_

#include "ZZUtils.h"
using namespace ZZ;

#include "GameItem.h"

class GameItemView : public CCNode 
{
	CCLabelTTF* m_lblName;
	CCLabelTTF* m_lblType;

	GameItem* m_pItem;

	virtual void initView();

public:
	GameItemView(void);
	~GameItemView(void);

	void setItem( GameItem* item );
	GameItem* getItem() { return m_pItem; } 
};


#endif
