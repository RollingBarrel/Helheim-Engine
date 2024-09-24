#include "BombBoss.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include "DecalComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "EnemyBoss.h"
#include "Physics.h"
#include "Geometry/Ray.h"
#include <cmath>


CREATE(BombBoss)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mRadius);
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
	decal = static_cast<DecalComponent*>(mGameObject->GetComponent(ComponentType::DECAL));
	for (GameObject* go : mGameObject->GetChildren())
	{
		if (go->GetName() == "Bomb")
		{
			mBombGO = go;
			break;
		}
	}

	std::vector<GameObject*> rootChildren = App->GetScene()->GetRoot()->GetChildren();
	for (GameObject* go : rootChildren)
	{
		if (go->GetName() == "FinalBoss")
		{
			mTimeDelay = static_cast<EnemyBoss*>(static_cast<ScriptComponent*>(go->GetComponent(ComponentType::SCRIPT))->GetScriptInstance())->GetBombsDelay();
			break;
		}
	}
}

void BombBoss::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;
	
	mTimePassed += App->GetDt();
	//log decal fade
	LOG("Decal fade: %f", decal->GetFadeFactor());

	if (mTimePassed < mTimeDelay)
	{
		decal->SetFadeFactor(mTimePassed / mTimeDelay);
	}
	else if (mTimePassed >= mTimeDelay && !mHasExploded)
	{
		GameObject* player = GameManager::GetInstance()->GetPlayer();
		float3 playerPosition = player->GetWorldPosition();
		float distanceToCenter = (playerPosition - mGameObject->GetWorldPosition()).Length();
		for (Component* particlecomponent : mExplosionParticles)
		{
			particlecomponent->GetOwner()->SetEnabled(true);
		}
		if (distanceToCenter <= mRadius)
		{
			PlayerController* playerScript = (PlayerController*)((ScriptComponent*)player->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
			playerScript->TakeDamage(mDamage);
		}
		mHasExploded = true;
	}
	else if (mTimePassed >= mTimeDelay && mHasExploded)
	{
		bool finishedExploding = true;
		for (Component* particlecomponent : mExplosionParticles)
		{
			finishedExploding = static_cast<ParticleSystemComponent*>(particlecomponent)->HasEnded();
		}
		if (finishedExploding)
		{
			decal->SetFadeFactor(1.0f - (mTimePassed - (mTimeDelay + 2)));
			if (decal->GetFadeFactor() <= 0.f)
			{
				mGameObject->SetEnabled(false);
			}
		}
	}
}

void BombBoss::Init(float3 bombOrigin, float damage)
{
	mGameObject->SetEnabled(true);
	mHasExploded = false;
	mTimePassed = 0.0f;
	mBombOrigin = bombOrigin;
	mDamage = damage;
	for (Component* particlecomponent : mExplosionParticles)
	{
		particlecomponent->GetOwner()->SetEnabled(false);
	}
	mGameObject->SetWorldScale(float3(mRadius * 2));
}