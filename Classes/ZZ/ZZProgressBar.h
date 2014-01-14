#ifndef _ZZPROGRESSBAR_H_
#define _ZZPROGRESSBAR_H_

#include <cocos2d.h>

using namespace cocos2d;

namespace ZZ {

class ProgressBar : public CCNode
{
	CCLayerColor* m_bg;
	CCLayerColor* m_bar;
	float m_margin;

public:
	ProgressBar(void);
	virtual ~ProgressBar(void);

	void setColor( const ccColor3B& fg, const ccColor3B& bg );

	virtual bool init(  const CCRect &area );

	static ProgressBar* create( const CCRect &area );

	void setProgress( float pct );
	void setMargin( float margin );
};

}

#endif
