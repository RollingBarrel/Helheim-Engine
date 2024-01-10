#include "Component.h"
#include "GameObject.h"


int Component::lastcomponentIndex = 0;

Component::Component(const char* name, GameObject* owner, ComponentType type) 
	:mName(name), mOwner(owner), mType(type), mIsEnabled(true), mPopupID("ComponentOptions_"+ lastcomponentIndex++)
{}