#include "BombBoss.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
CREATE(BombBoss)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mArea);
	MEMBER(MemberType::FLOAT, mDamage);
	MEMBER(MemberType::FLOAT, mTimeDelay);
	END_CREATE;
}

BombBoss::BombBoss(GameObject* owner) : Script(owner)
{
}

BombBoss::~BombBoss()
{
}

void BombBoss::Start()
{
	mGameObject->GetComponentsInChildren(ComponentType::PARTICLESYSTEM, mExplosionParticles);
	for (Component* particlecomponent : mExplosionParticles)
	{
		particlecomponent->GetOwner()->SetEnabled(false);
	}

}


void BombBoss::Update()
{
	mTimePassed += App->GetDt();
	/*LOG("BUUULEEETT");*/
	if (mHasExploded)
	{
		bool finishedExploding = true;
		for (Component* particlecomponent : mExplosionParticles)
		{
			finishedExploding *= reinterpret_cast<ParticleSystemComponent*>(particlecomponent)->HasEnded();
		}
		if (finishedExploding)
		{
			mGameObject->SetEnabled(false);
		}
	}
	else if (mTimePassed >= mTimeDelay)
	{
		GameObject* player = GameManager::GetInstance()->GetPlayer();
		float3 playerPosition = player->GetWorldPosition();
		float distanceToCenter = (playerPosition - mGameObject->GetWorldPosition()).Length();
		for (Component* particlecomponent : mExplosionParticles)
		{
			particlecomponent->GetOwner()->SetEnabled(true);
		}
		if (distanceToCenter <= mArea)
		{
			PlayerController* playerScript = (PlayerController*)((ScriptComponent*)player->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
			playerScript->TakeDamage(mDamage);
		}
		mHasExploded = true;
	}
}

void BombBoss::Init()
{
	mHasExploded = false;
	mTimePassed = 0.0f;
	for (Component* particlecomponent : mExplosionParticles)
	{
		particlecomponent->GetOwner()->SetEnabled(false);
	}

}