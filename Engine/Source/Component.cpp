#include "Component.h"
#include "GameObject.h"

int Component::mLastcomponentIndex = 0;

Component::Component(const char* name, GameObject* owner, ComponentType type)
	:mName(name), mOwner(owner), mType(type), mIsEnabled(true), mComponentIndex(mLastcomponentIndex++),
	mPopupID("ComponentOptions_" + mComponentIndex){}