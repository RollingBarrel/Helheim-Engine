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
#include "GameManager.h"


CREATE(StateMachineTest) 
{
	CLASS(owner)
	MEMBER(MemberType::GAMEOBJECT, mGameManagerGO);
	MEMBER(MemberType::FLOAT, mPlayerSpeed);
	MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);

}



StateMachineTest::StateMachineTest(GameObject* owner) : Script(owner)
{
}

void StateMachineTest::Start()
{
	if (mGameManagerGO)
	{
		ScriptComponent* script = (ScriptComponent*)mGameManagerGO->GetComponent(ComponentType::SCRIPT);
		mGameManager = (GameManager*)script->GetScriptInstance();
	}
}

void StateMachineTest::Update()
{
    switch (mCurrentState)
    {
    case AnimationState::IDLE:
        Idle();
        break;
   
    case AnimationState::WALK:
        Moving();

        break;
    case AnimationState::DIE:
        Die();
        break;
    }
}

void StateMachineTest::Idle()
{
    //mAnimationComponent->SendTrigger("Idle");

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mCurrentState = AnimationState::WALK;
    }
    else
    {
        mCurrentState = AnimationState::IDLE;
    }

}

void StateMachineTest::Moving()
{
    //mAnimationComponent->SendTrigger("Walk");

    Idle();
}

void StateMachineTest::Die()
{
    //mAnimationComponent->SendTrigger("Die");
}
