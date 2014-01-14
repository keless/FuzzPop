#include "GameItemView.h"


GameItemView::GameItemView(void)
{
	m_pItem = NULL;

	initView();
}


GameItemView::~GameItemView(void)
{
}


//virtual 
void GameItemView::initView()
{
	int cardW = 200;
	int cardH = 200;
	setContentSize( CCSizeMake( cardW, cardH ));
	setAnchorPoint(ccp(0.5f,0.5f));

	CCLayerColor* bg = CCLayerColor::create(ccc4(25,25,25,155), cardW,cardH);
	bg->setAnchorPoint(ccp(0,0));
	addChild(bg, 1);

	m_lblName = CCLabelTTF::create();
	m_lblName->initWithString( "", "Arial", 24.0f  );
	m_lblName->setAnchorPoint(ccp(0.0f, 1.0f ) );
	m_lblName->setPositionY(cardH);
	addChild(m_lblName, 50);

	m_lblType = CCLabelTTF::create();
	m_lblType->initWithString( "", "Arial", 24.0f  );
	m_lblType->setAnchorPoint(ccp(0.0f, 1.0f ) );
	m_lblType->setPositionY(cardH - 50);
	addChild(m_lblType, 50);

}


void GameItemView::setItem( GameItem* item )
{
	if( m_pItem != NULL ) 
	{
		CC_SAFE_RELEASE_NULL(m_pItem);
	}

	m_pItem = item;

	if( m_pItem != NULL ) {

		m_pItem->retain();

		m_lblName->setString( item->getName().c_str() );
	
		m_lblType->setString( GameItem::itemTypeToString( item->getType() ).c_str() );
	}else {
		m_lblName->setString("");
		m_lblType->setString("none");
	}

}