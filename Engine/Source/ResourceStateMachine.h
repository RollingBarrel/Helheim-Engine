#pragma once
#include "Resource.h"

class AnimationStateMachine;

class ENGINE_API ResourceStateMachine :
    public Resource
{
public:
    ResourceStateMachine(unsigned int uid, AnimationStateMachine* sm);
    ~ResourceStateMachine();

    AnimationStateMachine* GetStateMachine() const { return mStateMachine; }
    void SetStateMachine(AnimationStateMachine* statemachine) { mStateMachine = statemachine; }
private:

    AnimationStateMachine* mStateMachine;

};

