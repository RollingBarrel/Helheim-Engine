#include "EnemyCreatureMelee.h"
#include "Application.h"
#include "GameObject.h"

#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "AIAGentComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"

CREATE(EnemyCreatureMelee)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRotationSpeed);
	MEMBER(MemberType::INT, mShieldDropRate);
	MEMBER(MemberType::FLOAT, mMeeleDamage);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);

	END_CREATE;
}

void EnemyCreatureMelee::Attack()
{
    Enemy::Attack();

    if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
    {
        float3 playerPosition = mPlayer->GetWorldPosition();
        float distanceToEnemy = (playerPosition - mGameObject->GetWorldPosition()).Length();
        float3 enemyFrontNormalized = mGameObject->GetFront().Normalized();
        float3 playerToEnemy = (mGameObject->GetWorldPosition() - playerPosition).Normalized();
        float dotProduct = playerToEnemy.Dot(enemyFrontNormalized);

        if (distanceToEnemy < 2.0f && dotProduct < 0)
        {
            PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (playerScript != nullptr)
            {
                playerScript->TakeDamage(mMeeleDamage);
            }
        }
    }
}

void EnemyCreatureMelee::Death()
{
    if (mAnimationComponent)
    {
        mAnimationComponent->SendTrigger("tDeath", 0.3f);
    }

    if (mAiAgentComponent)
    {
        mAiAgentComponent->PauseCrowdNavigation();
    }

    if (mDeathTimer.Delay(1.4f))
    {
        Enemy::Death();
    }
}