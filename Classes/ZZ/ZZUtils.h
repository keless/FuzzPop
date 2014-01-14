#ifndef _ZZUTILS_H_
#define _ZZUTILS_H_

#include <cocos2d.h>

using namespace cocos2d;

#include "json.h"
#include "ZZProgressBar.h"
#include "ZZEventBus.h"
#include "ZZRadialLayer.h"
#include "ZZTouchableNode.h"
#include "ZZTextField.h"
#include "ZZRadioGroup.h"
#include "ZZJsonManager.h"
#include "ZZAnimatedSprite.h"

namespace ZZ {


bool IsFile( const std::string& filename );
Json::Value ReadFileToJson( const std::string& fileName );
bool WriteJsonToFile( const Json::Value& json, const std::string& fileName );
CCLayer* CreateSimplePopup( std::string msg, std::string btnLabel, std::string okEvt, std::string bus = "game" );
CCLayer* CreateSimpleEditBox( std::string msg, std::string defaultTxt, std::string btnLabel, std::string okEvt, std::string bus = "game" );
TouchableNode* CreateSimpleButton( std::string msg, std::string evt, std::string bus = "game" );
CCLabelTTF* CreateSimpleLabel( std::string label );
CCNode* CreateSimpleNamedLabel( std::string labelName, std::string labelText );

bool rHasInvisibleParent( CCNode* node );

}

#endif
