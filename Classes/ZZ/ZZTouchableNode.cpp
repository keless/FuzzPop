#include "ZZTouchableNode.h"
#include "ZZUtils.h"


namespace ZZ {

TouchableNode::TouchableNode( std::string evt, std::string bus) : m_evt(evt), m_bus(bus) 
{
	setTouchMode(kCCTouchesOneByOne);
	setTouchEnabled(true);
	ignoreAnchorPointForPosition(false);
	setAnchorPoint(ccp(0.5,0.5));
	m_touchStarted = false;
	m_pKillOnTouch = NULL;
	m_pGrabStringOnTouch = NULL;
}

/*
void TouchableNode::addChild(CCNode *child, int zOrder, int tag)
{
    CCNode::addChild(child, zOrder, tag);

    CCRect childArea = child->boundingBox();
	CCRect area = this->boundingBox();

	//todo: resize to fit children?

}
*/

//virtual 
bool TouchableNode::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!isVisible()) return false;

	CCPoint p = pTouch->getLocation();
	//todo: convert to local coords

	CCPoint local = convertToNodeSpace(p);

	for( int i=0; i< m_pChildren->count(); i++)
	{
		CCNode* child = (CCNode*) m_pChildren->objectAtIndex(i);
		if( child->boundingBox().containsPoint( local ) ) {
			m_touchStarted = true;
			return true;
		}
	}

	return false;
}
//virtual 
void TouchableNode::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	CCPoint p = pTouch->getLocation();
	//todo: convert to local coords

	CCPoint local = convertToNodeSpace(p);

	bool movedInside = false;

	for( int i=0; i< m_pChildren->count(); i++)
	{
		CCNode* child = (CCNode*) m_pChildren->objectAtIndex(i);
		if( child->boundingBox().containsPoint( local ) ) {
			movedInside = true;
			break;
		}
	}
}

//virtual 
void TouchableNode::onTouched()
{
	//handle touch event
	CCLog("send %s:%s", m_bus.c_str(),m_evt.c_str());
	JsonEvent* evt = new JsonEvent(m_evt);
	evt->json = m_data;

	if( m_pGrabStringOnTouch != NULL ) 
	{
		const char* str = m_pGrabStringOnTouch->getString();
		if( str != NULL ) 
			evt->json["string"] = str;
	}

	if( m_evt.size() > 0  ) {
		EventBus::get(m_bus.c_str())->dispatch(m_evt, evt );
	}

	if( m_pKillOnTouch != NULL ) {
		m_pKillOnTouch->removeFromParentAndCleanup(true);
	}
}

//virtual 
void TouchableNode::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(!isVisible()) return;

	if( rHasInvisibleParent(this) ) return; //recursive search up to root to see if any parents are invisible

	CCPoint p = pTouch->getLocation();
	//todo: convert to local coords

	CCPoint local = convertToNodeSpace(p);

	for( int i=0; i< m_pChildren->count(); i++)
	{
		CCNode* child = (CCNode*) m_pChildren->objectAtIndex(i);
		if( child->boundingBox().containsPoint( local ) ) {
			//touched inside

			onTouched();

			return;
		}
	}

	m_touchStarted = false;
}
//virtual 
void TouchableNode::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent)
{
	m_touchStarted = false;
}

}