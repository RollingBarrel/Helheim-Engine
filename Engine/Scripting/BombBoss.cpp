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
	// Check with a raycast if there's floor under the bomb
	Ray ray;
	ray.pos = mGameObject->GetWorldPosition() + float3(0, 10, 0);
	ray.dir = float3(0, -1, 0);
	Hit hit;

	Physics::Raycast(hit, ray, 50);
	LOG("Bomb origin: %f %f %f", mGameObject->GetWorldPosition().x, mGameObject->GetWorldPosition().y, mGameObject->GetWorldPosition().z);
	LOG("Raycast position: %f %f %f", ray.pos.x, ray.pos.y, ray.pos.z);
	LOG("Raycast hit point: %f %f %f", hit.mHitPoint.x, hit.mHitPoint.y, hit.mHitPoint.z);
	LOG("Raycast hit: %s", hit.IsValid() ? "TRUE" : "FALSE");
	LOG("-----------------------------------");

	if (hit.IsValid())
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
	else
	{
		mGameObject->SetEnabled(false);
	}
}