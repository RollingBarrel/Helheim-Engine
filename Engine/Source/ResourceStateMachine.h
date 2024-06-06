#pragma once
#include "Resource.h"

class AnimationStateMachine;

class ResourceStateMachine :
    public Resource
{
public:
    ResourceStateMachine(unsigned int uid, AnimationStateMachine* sm);
    ~ResourceStateMachine();

    const AnimationStateMachine* GetStateMachine() const { return mStateMachine; }

private:

    AnimationStateMachine* mStateMachine;

};

