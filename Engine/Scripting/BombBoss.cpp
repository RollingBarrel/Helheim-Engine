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
	for (GameObject* go : mGameObject->GetChildren())
	{
		if (go->GetName() == "Bomb")
		{
			mBombGO = go;
			break;
		}
	}
}

void BombBoss::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;
	
	mTimePassed += App->GetDt();
	if (mHasExploded)
	{
		bool finishedExploding = true;
		for (Component* particlecomponent : mExplosionParticles)
		{
			finishedExploding = static_cast<ParticleSystemComponent*>(particlecomponent)->HasEnded();
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
		if (distanceToCenter <= mRadius)
		{
			PlayerController* playerScript = (PlayerController*)((ScriptComponent*)player->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
			playerScript->TakeDamage(mDamage);
		}
		mHasExploded = true;
	}
}

void BombBoss::Init(float3 bombOrigin, float damage)
{
	mGameObject->SetEnabled(true);
	mHasExploded = false;
	mTimePassed = 0.0f;
	//Takes the value mTimeDelay from the object called FinalBoss in the root's children
	std::vector<GameObject*> rootChildren = App->GetScene()->GetRoot()->GetChildren();
	for (GameObject* go : rootChildren)
	{
		if (go->GetName() == "FinalBoss")
		{
			mTimeDelay = static_cast<EnemyBoss*>(static_cast<ScriptComponent*>(go->GetComponent(ComponentType::SCRIPT))->GetScriptInstance())->GetBombsDelay();
			break;
		}
	}
	mBombOrigin = bombOrigin;
	mDamage = damage;
	for (Component* particlecomponent : mExplosionParticles)
	{
		particlecomponent->GetOwner()->SetEnabled(false);
	}
	mGameObject->SetWorldScale(float3(mRadius*2));
}