#include "BossLaser.h"
#include "Application.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "MathFunc.h"
#include "State.h"
#include "Geometry/Ray.h"

CREATE(BossLaser)
{
	CLASS(owner);
	MEMBER(MemberType::FLOAT, mAngle);
	MEMBER(MemberType::FLOAT, mDamage);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRange);

	SEPARATOR("GAME OBJECTS");
	MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
	MEMBER(MemberType::GAMEOBJECT, mLaserTrail);
	MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
	MEMBER(MemberType::GAMEOBJECT, mLaserCharge);

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
		Ray ray;
		ray.pos = mLaserOrigin->GetWorldPosition();
		ray.pos.y++;
		ray.dir = mGameObject->GetFront();

		Hit hit;
		Physics::Raycast(hit, ray, mRange);
		if (hit.IsValid())
		{
			if (hit.mGameObject->GetTag() == "Player" && mIframes == 0)
			{
				mIframes = 10.0f;
				LOG("Collided with player");
				ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
				PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());
				if (player->GetPlayerLowerState()->GetType() != StateType::DASH)
				{
					player->TakeDamage(mDamage);
				}
			}

			mLaserEnd->SetWorldPosition(hit.mHitPoint);
			//Trails WorkAround
			if (mMoveTrail)
			{
				mLaserTrail->SetWorldPosition(hit.mHitPoint);
				mMoveTrail = false;
			}
			else
			{
				mMoveTrail = true;
				mLaserTrail->SetWorldPosition(mLaserOrigin->GetWorldPosition());
			}


		}
		else
		{
			float3 originPosition = mLaserOrigin->GetWorldPosition();
			//mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mRange));
			mLaserEnd->SetWorldPosition(originPosition + mGameObject->GetFront() * mRange);

			//Trails WorkAround
			if (mMoveTrail)
			{
				//mLaserTrail->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mRange));
				mLaserTrail->SetWorldPosition(originPosition + mGameObject->GetFront() * mRange);
				mMoveTrail = false;
			}
			else
			{
				//mLaserTrail->SetLocalPosition(originPosition);
				mLaserTrail->SetWorldPosition(originPosition);
				mMoveTrail = true;
			}
		}
	}
	if (mIframes > 0) --mIframes;
}

void BossLaser::Init(float damage, float range)
{
	mSwipeProgress = 0.0f;
	mGameObject->SetEnabled(true);
	mGameObject->SetLocalRotation(float3(0, DegToRad(-mAngle / 2), 0));
	mDamage = damage;
	mRange = range;
}

void BossLaser::OnCollisionEnter(CollisionData* collisionData)
{
	//if (collisionData->collidedWith->GetTag().compare("Player") == 0)
	//{
	//	LOG("Collided with player");
	//	ScriptComponent* playerScript = reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
	//	PlayerController* player = reinterpret_cast<PlayerController*>(playerScript->GetScriptInstance());
	//	if (player->GetPlayerLowerState()->GetType() != StateType::DASH)
	//	{
	//		player->TakeDamage(mDamage);
	//	}
	//}
}