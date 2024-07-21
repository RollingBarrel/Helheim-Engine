#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "MathFunc.h"
#include "State.h"

CREATE(BossLaser)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mAngle);
	MEMBER(MemberType::FLOAT, mDamage);
	MEMBER(MemberType::FLOAT, mSpeed);
	END_CREATE;
}

BossLaser::BossLaser(GameObject* owner) : Script(owner)
{
}

BossLaser::~BossLaser()
{
}

void BossLaser::Start()
{
	mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&BossLaser::OnCollisionEnter, this, std::placeholders::_1)));
	}
}


void BossLaser::Update()
{
	float dt = App->GetDt();
	if (mSwipeProgress > mAngle)
	{
		mGameObject->SetEnabled(false);
	}
	else
	{
		mSwipeProgress += dt * mSpeed;
		float newAngle = mSwipeProgress - (mAngle / 2);
		mGameObject->SetLocalRotation(float3(0, DegToRad(newAngle), 0));
	}
}

void BossLaser::Init()
{
	mSwipeProgress = 0.0f;
	mGameObject->SetEnabled(true);
	mGameObject->SetLocalRotation(float3(0, DegToRad(-mAngle / 2), 0));
}

void BossLaser::OnCollisionEnter(CollisionData* collisionData)
{
	if (collisionData->collidedWith->GetTag().compare("Player") == 0)
	{
		LOG("Collided with player");
		ScriptComponent* playerScript = reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
		PlayerController* player = reinterpret_cast<PlayerController*>(playerScript->GetScriptInstance());
		if (player->GetPlayerLowerState()->GetType() != StateType::DASH)
		{
			player->TakeDamage(mDamage);
		}
	}
}