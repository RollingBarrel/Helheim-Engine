#include "Component.h"

Component::Component(GameObject* owner, ComponentType type) : mOwner(owner), mType(type), mIsEnabled(true)
{
}
