#include "StateMachineTest.h"
#include "StateMachineTest.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "Geometry/Plane.h"
#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "GameManager.h"
#include "GameObject.h"



CREATE(StateMachineTest)
{
    CLASS(owner)
        MEMBER(MemberType::FLOAT, mPlayerSpeed);

    END_CREATE;
}



StateMachineTest::StateMachineTest(GameObject* owner) : Script(owner)
{
}

void StateMachineTest::Start()
{


    mAnimationComponent = (AnimationComponent*)mGameObject->GetComponent(ComponentType::ANIMATION);
    mStateMachine = mAnimationComponent->GetStateMachine();

    std::string defaultState = "default";
    std::string clip = "Zombunny";
    std::string stateIdle = "Idle";
    std::string stateWalk = "Walk";
    std::string walkTrigger = "tWalk";
    std::string idleTrigger = "tIdle";

    mStateMachine->SetClipName(0, clip);

    mStateMachine->AddState(clip, stateIdle);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(stateIdle), 1.25);
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(stateIdle), 12.0);

    mStateMachine->AddState(clip, stateWalk);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(stateWalk), 0.0);
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(stateWalk), 1.25);

    mStateMachine->AddTransition(defaultState, stateIdle, idleTrigger);
    mStateMachine->AddTransition(stateIdle, stateWalk, walkTrigger);
    mStateMachine->AddTransition(stateWalk, stateIdle, idleTrigger);

    mAnimationComponent->OnStart();
    mAnimationComponent->SetIsPlaying(true);
}

void StateMachineTest::Update()
{
    switch (mCurrentState)
    {
    case AnimationStates::IDLE:
        Idle();
        break;

    case AnimationStates::WALK:
        Moving();

        break;
    case AnimationStates::DIE:
        Die();
        break;
    }
}

void StateMachineTest::Idle()
{
    

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mCurrentState = AnimationStates::WALK;
    }
    else
    {
        mCurrentState = AnimationStates::IDLE;
        mAnimationComponent->SendTrigger("tIdle");
    }

}

void StateMachineTest::Moving()
{

    mAnimationComponent->SendTrigger("tWalk");

    Idle();
}

void StateMachineTest::Die()
{
    //mAnimationComponent->SendTrigger("Die");
}