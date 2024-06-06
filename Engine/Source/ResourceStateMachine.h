#pragma once
#include "Resource.h"

class AnimationStateMachine;

class ENGINE_API ResourceStateMachine :
    public Resource
{
public:
    ResourceStateMachine(unsigned int uid, AnimationStateMachine* sm);
    ~ResourceStateMachine();

    const AnimationStateMachine* GetStateMachine() const { return mStateMachine; }

private:

    AnimationStateMachine* mStateMachine;

};

