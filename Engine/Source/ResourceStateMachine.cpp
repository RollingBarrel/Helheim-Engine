#include "ResourceStateMachine.h"

ResourceStateMachine::ResourceStateMachine(unsigned int uid, AnimationStateMachine* sm) : Resource(uid, Resource::Type::StateMachine)
{
	mStateMachine = sm;
}

ResourceStateMachine::~ResourceStateMachine()
{
}
