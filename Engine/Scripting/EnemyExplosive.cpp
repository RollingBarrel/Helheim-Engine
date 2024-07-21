#include "EnemyExplosive.h"
#include "Application.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "PlayerController.h"

CREATE(EnemyExplosive)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);;
	MEMBER(MemberType::FLOAT, mAttackDamage);
	MEMBER(MemberType::FLOAT, mChargeDuration);
	MEMBER(MemberType::FLOAT, mExplosionRadius);
	SEPARATOR("STATES");
	MEMBER(MemberType::FLOAT, mAttackDistance);
	SEPARATOR("GAMEOBJECTS");
	MEMBER(MemberType::GAMEOBJECT, mExplosionWarningGO);
	END_CREATE;
}

void EnemyExplosive::Start()
{
	Enemy::Start();

	if (mExplosionWarningGO)
	{
		mWarningSize = mExplosionWarningGO->GetWorldScale();
		mExplosionWarningGO->SetLocalPosition(float3(0.0f, 0.1f, 0.0f));
		mExplosionWarningGO->SetEnabled(false);
	}
}

void EnemyExplosive::Charge()
{
	Enemy::Charge();

	ChargeWarningArea();
}

void EnemyExplosive::Attack()
{
	LOG("BOOM");
	mExplosionWarningGO->SetWorldScale(float3(0.1f));
	if (IsPlayerInRange(mExplosionRadius))
	{
		PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
		if (playerScript != nullptr)
		{
			playerScript->TakeDamage(mAttackDamage);
		}
	}
	TakeDamage(mMaxHealth);
}

void EnemyExplosive::ChargeWarningArea()
{
	float increment = (mExplosionRadius - mExplosionWarningGO->GetWorldScale().x) * App->GetDt();
	float3 newWarningSize = float3(mWarningSize.x += increment, mWarningSize.y, mWarningSize.z += increment);

	mExplosionWarningGO->SetWorldScale(newWarningSize);
}



