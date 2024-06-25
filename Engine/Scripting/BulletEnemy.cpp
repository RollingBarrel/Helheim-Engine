#include "BulletEnemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "ObjectPool.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "ParticleSystemComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
CREATE(BulletEnemy)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mDamage);
	END_CREATE;
}

BulletEnemy::BulletEnemy(GameObject* owner) : Script(owner)
{
}

BulletEnemy::~BulletEnemy()
{
}

void BulletEnemy::Start()
{
	mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&BulletEnemy::OnCollisionEnter, this, std::placeholders::_1)));
	}

	mHitParticles = reinterpret_cast<ParticleSystemComponent*>(mGameObject->GetComponent(ComponentType::PARTICLESYSTEM));
	if (mHitParticles)
	{
		//mHitParticles->SetEnabled(false);
	}

}


void BulletEnemy::Update()
{
	/*LOG("BUUULEEETT");*/
	if (!mHasCollided)
	{
		if (mTotalMovement <= mRange)
		{
			mTotalMovement += mGameObject->GetWorldPosition().Distance((mGameObject->GetWorldPosition() + mGameObject->GetFront().Mul(mSpeed)));
			mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + mGameObject->GetFront() * mSpeed);
			//LOG("Position: %f,%f,%f ", mGameObject->GetPosition().x, mGameObject->GetPosition().y, mGameObject->GetPosition().z);
		}
		else
		{
			mGameObject->SetEnabled(false);
		}
	}
	else
	{
		if (Delay(1.0f))
		{
			mGameObject->SetEnabled(false);
		}
	}
}

void BulletEnemy::Init()
{
	mHasCollided = false;
	mTotalMovement = 0.0f;
}


void BulletEnemy::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Player") == 0)
	{
		LOG("Collided with player")
		ScriptComponent* playerScript = reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
		PlayerController* player = reinterpret_cast<PlayerController*>(playerScript->GetScriptInstance());
		player->TakeDamage(mDamage);
		if (mHitParticles)
		{
			//mHitParticles->SetEnabled(true);
		}
		mHasCollided = true;
		mGameObject->SetEnabled(false);
	}
}

bool BulletEnemy::Delay(float delay)
{
	mTimePassed += App->GetDt();

	if (mTimePassed >= delay)
	{
		mTimePassed = 0;
		return true;
	}
	else return false;
}