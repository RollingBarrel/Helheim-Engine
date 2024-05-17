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

    
    std::string clip = "Character";

    std::string defaultState = "default";
    std::string sIdle = "Idle";
    std::string sWalkForward = "Walk Forward";
    std::string sWalkBack = "Walk Back";
    std::string sStrafeLeft = "Strafe Left";
    std::string sStrafeRight = "Strafe Right";
    std::string sShooting = "Shooting";

    std::string idleTrigger = "tIdle";
    std::string forwardTrigger = "tWalkForward";
    std::string backTrigger = "tWalkBack";
    std::string strafeLeftTrigger = "tStrafeLeft";
    std::string strafeRightTrigger = "tStrafeRight";


    mStateMachine->SetClipName(0, clip);

    //States
    mStateMachine->AddState(clip, sIdle);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sIdle), float(6.03));
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sIdle), float(12.0));
    
    mStateMachine->AddState(clip, sWalkForward);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkForward), float(2.53));
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkForward), float(3.52));

    mStateMachine->AddState(clip, sWalkBack);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkBack), float(3.53));
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkBack), float(4.52));

    mStateMachine->AddState(clip, sStrafeLeft);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeLeft), float(0.0));
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeLeft), float(1.26));

    mStateMachine->AddState(clip, sStrafeRight);
    mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeRight), float(1.27));
    mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeRight), float(2.52));

    //Transitions
    mStateMachine->AddTransition(defaultState, sIdle, idleTrigger);


    mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
    mStateMachine->AddTransition(sIdle, sWalkBack, backTrigger);
    mStateMachine->AddTransition(sIdle, sStrafeLeft, strafeLeftTrigger);
    mStateMachine->AddTransition(sIdle, sStrafeRight, strafeRightTrigger);

    mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
    mStateMachine->AddTransition(sWalkForward, sWalkBack, backTrigger);
    mStateMachine->AddTransition(sWalkForward, sStrafeLeft, strafeLeftTrigger);
    mStateMachine->AddTransition(sWalkForward, sStrafeRight, strafeRightTrigger);

    mStateMachine->AddTransition(sWalkBack, sIdle, idleTrigger);
    mStateMachine->AddTransition(sWalkBack, sWalkForward, sWalkForward);
    mStateMachine->AddTransition(sWalkBack, sStrafeLeft, strafeLeftTrigger);
    mStateMachine->AddTransition(sWalkBack, sStrafeRight, strafeRightTrigger);

    mStateMachine->AddTransition(sStrafeLeft, sIdle, idleTrigger);
    mStateMachine->AddTransition(sStrafeLeft, sWalkForward, sWalkForward);
    mStateMachine->AddTransition(sStrafeLeft, sWalkBack, backTrigger);
    mStateMachine->AddTransition(sStrafeLeft, sStrafeRight, strafeRightTrigger);

    mStateMachine->AddTransition(sStrafeRight, sIdle, idleTrigger);
    mStateMachine->AddTransition(sStrafeRight, sWalkForward, sWalkForward);
    mStateMachine->AddTransition(sStrafeRight, sWalkBack, backTrigger);
    mStateMachine->AddTransition(sStrafeRight, sStrafeLeft, strafeLeftTrigger);

    mAnimationComponent->OnStart();
    mAnimationComponent->SetIsPlaying(true);
}

void StateMachineTest::Update()
{
    switch (mCurrentState)
    {
    case AnimationStates::IDLE:
        mAnimationComponent->SendTrigger("tIdle", 0.01);
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
        
    }

}

void StateMachineTest::Moving()
{
    
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT )
    {
        mAnimationComponent->SendTrigger("tWalkForward",0.0);
    }
    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        mAnimationComponent->SendTrigger("tWalkBack", 0.0);
    }
    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        mAnimationComponent->SendTrigger("tStrafeLeft", 0.0);
    }
    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mAnimationComponent->SendTrigger("tStrafeRight", 0.0);
    }
    

    Idle();
}

void StateMachineTest::Die()
{
    //mAnimationComponent->SendTrigger("Die");
}