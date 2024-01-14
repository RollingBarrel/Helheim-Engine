#include "Component.h"
#include "GameObject.h"
#include "Algorithm/Random/LCG.h"

Component::Component(const char* name, GameObject* owner, ComponentType type)
	:mName(name), mOwner(owner), mType(type), mIsEnabled(true),
	mID(LCG().Int()){}

