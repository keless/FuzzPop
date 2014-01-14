#ifndef _ZZRADIALLAYER_H_
#define _ZZRADIALLAYER_H_

#include <cocos2d.h>

using namespace cocos2d;

#include "ZZTouchableNode.h"

namespace ZZ {

class RadialLayer : public CCNode
{
	CCDrawNode* m_bg;

	CCNode* m_center;

	std::vector<TouchableNode*> m_items;

	float m_radius;
	float m_tierRadius;

public:
	RadialLayer(void);
	virtual ~RadialLayer(void);

	static RadialLayer* create();

	virtual bool init( float radius = 150, float tierRadius = 75 );  

	void setCenterNode( CCNode* node );
	CCNode* getCenterNode() { return m_center; }

	void addItem( CCNode* item, std::string evt, std::string bus = "game" );
	void addItemWithJson( CCNode* item, std::string evt, JsonValueRef json, std::string bus = "game" );
};

}

#endif
