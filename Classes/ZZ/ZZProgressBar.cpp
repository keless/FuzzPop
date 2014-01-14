#include "ZZProgressBar.h"

namespace ZZ {

ProgressBar::ProgressBar(void) : m_bg(NULL), m_bar(NULL), m_margin(0)
{
}


ProgressBar::~ProgressBar(void)
{
}

//virtual 
bool ProgressBar::init( const CCRect &area )
{
	setContentSize( CCSizeMake( area.size.width, area.size.height ) );

	m_bg = CCLayerColor::create( ccc4(255,0,0,255), area.size.width, area.size.height );
	m_bar = CCLayerColor::create( ccc4(0,255,0,255), area.size.width, area.size.height );
	m_bar->setAnchorPoint(ccp(0.0f, 0.5f));
	//m_bar->setPositionX( area.size.width * -0.5f );
	//m_bar->setPositionY( 30 );

	addChild(m_bg);
	addChild(m_bar);

	this->setPosition(area.origin.x, area.origin.y);

	return true;
}

//.static 
ProgressBar * ProgressBar::create( const CCRect &area )
{
	ProgressBar* bar = new ProgressBar();
	if(!bar->init(area))
	{
		delete bar;
		return NULL;
	}
	bar->autorelease();
	return bar;
}

void ProgressBar::setColor( const ccColor3B& fg, const ccColor3B& bg )
{
	m_bg->setColor( bg );
	m_bar->setColor(fg);
}

void ProgressBar::setProgress( float pct )
{
	m_bar->setScaleX( pct );
}

void ProgressBar::setMargin( float margin )
{
	CCSize area = m_bg->getContentSize();
	area.height -= margin * 2;
	area.width -= margin * 2;
	m_margin = margin;
	m_bar->setContentSize(area);
	m_bar->setPosition(ccp(margin, margin )); //after resizing, cause position to recalculate
}

}
