#include "Rat.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
#include "AIAGentComponent.h"
#include "GameManager.h"


CREATE(Rat)
{
    CLASS(owner);
    END_CREATE;
}


void Rat::Update()
{
    Enemy::Update(); 
}

void Rat::Idle()
{
	if (IsPlayerInRange(mFleeRadius))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
		mCurrentState = EnemyState::FLEE;
	}
}

void Rat::Flee()
{
	PlayStepAudio();
	if (mAiAgentComponent)
	{
		float distance = mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition());
		float3 newDir = mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition();
		float collisionDotProduct = newDir.Dot(mEnemyCollisionDirection);
		if (collisionDotProduct < 0.0f)
		{
			newDir = newDir - mEnemyCollisionDirection.Mul(collisionDotProduct);
		}
		float3 newPos = mGameObject->GetWorldPosition() + newDir * mSpeed;
		mAiAgentComponent->SetNavigationPath(App->GetNavigation()->FindNearestPoint(newPos, float3(1.0f)));
		mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());

	}

	if (!IsPlayerInRange(mFleeRadius))
	{
		mCurrentState = EnemyState::IDLE;
	}
}
