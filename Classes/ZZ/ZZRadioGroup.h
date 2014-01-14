#ifndef _RADIOGROUP_H_
#define _RADIOGROUP_H_

#include "ZZTouchableNode.h"
#include "ZZEventBus.h"

/*************

RadioGroupResponder is a touchable node that sends events to other RGRs of the same group for them to toggle 'off' while it toggles on

you need to call initRadioGroup in order to connect an RGR to a group

when touched, it will send a JsonEvent named "zzrgrTrigger" on an EventBus with the radioGroup name,
	that JsonEvent will have an "index" representing which RGR item was touched

ex: 

//create a radio button
	BaseRadioGroupLayer* radio = new BaseRadioGroupLayer();
	radio->initRadioGroup("partyMemberSelect", idx);

//add a listener for the radio group
	EventBus::get("partyMemberSelect")->addListener("zzrgrTrigger", this, callfuncO_selector(PartyEditorScreen::onPartyMemberSelected));

//handle the event when one of the radio group buttons is selected
void PartyEditorScreen::onPartyMemberSelected(CCObject* e)
{
	JsonEvent* evt = dynamic_cast<JsonEvent*>(e);
	if(evt == NULL) return;

	int partyMemberIdx = evt->json.get("index", -1).asInt();
	CCLog("party member %d selected", partyMemberIdx);
}

**************/

namespace ZZ {

class RadioGroupResponder : public TouchableNode
{
protected:
	std::string m_radioGroupName;
	int m_radioGroupIdx;

public:
	RadioGroupResponder() : TouchableNode("")
	{
		m_radioGroupIdx = -1;
	}
	virtual ~RadioGroupResponder() {
		EventBus::get(m_radioGroupName.c_str())->remListener("zzrgrTrigger", this, callfuncO_selector(RadioGroupResponder::handleRadioGroupEvt));
	}

	virtual void onTouched()
	{
		triggerGroup();
	}

	void initRadioGroup(std::string radioGroupName, int idx) { 
		m_radioGroupIdx = idx;
		m_radioGroupName = radioGroupName; 
		EventBus::get(m_radioGroupName.c_str())->addListener("zzrgrTrigger", this, callfuncO_selector(RadioGroupResponder::handleRadioGroupEvt));
	}

	virtual void handleRadioGroupEvt( CCObject* e ) = 0;
	virtual void triggerGroup() { 
		if( m_radioGroupName.size() == 0 ) return;

		JsonEvent* evt = new JsonEvent("zzrgrTrigger");
		evt->json["index"] = m_radioGroupIdx;
		EventBus::get(m_radioGroupName.c_str())->dispatch(evt->type, evt); 
	}
};

class BaseRadioGroupLayer : public RadioGroupResponder
{
protected:
	CCLayerColor* m_highlight;

	static BaseRadioGroupLayer* create() { return NULL; } //hide inherited create

public:
	BaseRadioGroupLayer()
	{
		m_highlight = CCLayerColor::create(ccc4(255,255,255,255));
		addChild(m_highlight);
		m_highlight->setVisible(false);
	}
	//virtual ~BaseRadioGroupLayer();

	virtual void setContentSize( const CCSize& area )
	{
		CCNode::setContentSize(area);
		m_highlight->setContentSize(area);
	}

	virtual void handleRadioGroupEvt( CCObject* e )
	{
		m_highlight->setVisible(false); 
	}

	virtual void triggerGroup() {
		RadioGroupResponder::triggerGroup();
		m_highlight->setVisible(true); //handle the toggle AFTER sending (because we're going to toggle off as a result of sending)
	}


};

}

#endif

