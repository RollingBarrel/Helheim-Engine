#include "ExplosiveTrap.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "Enemy.h"


CREATE(ExplosiveTrap)
{
	CLASS(owner);
	SEPARATOR("SIZE");
	MEMBER(MemberType::FLOAT, mTriggerArea);
	MEMBER(MemberType::FLOAT, mExplosionArea);

	SEPARATOR("ACTIVATION PARAMETER");
	MEMBER(MemberType::FLOAT, mExplosionWait);
	MEMBER(MemberType::FLOAT, mExplosionDuration);

	SEPARATOR("SIDE EFFECT");
	MEMBER(MemberType::FLOAT, mDamageAmount);

	SEPARATOR("GAME OBJECT");
	MEMBER(MemberType::GAMEOBJECT, mExplosionPrestartVFX);
	MEMBER(MemberType::GAMEOBJECT, mExplosionVFX);
	MEMBER(MemberType::GAMEOBJECT, mBarrelMesh);
	END_CREATE;
}

void ExplosiveTrap::Start()
{
	mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
	if (mCollider)
	{
		mCollider->SetSize(float3(mTriggerArea));
		mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ExplosiveTrap::OnCollisionEnter, this, std::placeholders::_1)));
	}

	if (mExplosionPrestartVFX) mExplosionPrestartVFX->SetWorldScale(float3(mExplosionArea, mExplosionArea, 1.0f));
	if (mExplosionVFX) mExplosionVFX->SetWorldScale(float3(mExplosionArea));
}

void ExplosiveTrap::Update()
{
	switch (mState)
	{
	case TRAP_STATE::EXPLOSION_CHARGE:
		if (mExplosionChargeTimer.Delay(mExplosionWait))
		{
			mState = TRAP_STATE::EXPLOSION_START;
			if (mExplosionPrestartVFX) mExplosionPrestartVFX->SetEnabled(false);
			if (mExplosionVFX) mExplosionVFX->SetEnabled(true);
			mBarrelMesh->SetEnabled(false);

			GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_EXPLOSION, mGameObject->GetWorldPosition());
		}
		break;

	case TRAP_STATE::EXPLOSION_START:
		mState = TRAP_STATE::EXPLOSION_END;
		GameManager::GetInstance()->GetPlayerCamera()->ActivateShake(0.5f, 0.3f);
		DoDamage();
		break;

	case TRAP_STATE::EXPLOSION_END:
		if (mExplosionDurationTimer.Delay(mExplosionDuration)) mGameObject->SetEnabled(false);
		break;

	default:
		break;
	}
}

void ExplosiveTrap::OnCollisionEnter(CollisionData* collisionData)
{
	GameObject* collision = collisionData->collidedWith;

	if (mState == TRAP_STATE::INACTIVE)
	{
		if (collision->GetTag().compare("Player") == 0)
		{
			mState = TRAP_STATE::EXPLOSION_CHARGE;
			GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mGameObject->GetWorldPosition());

			mExplosionPrestartVFX->SetEnabled(true);
		}
	}
}

void ExplosiveTrap::InnerTrapTakeDamage()
{
	if (mState == TRAP_STATE::INACTIVE)
	{
		mState = TRAP_STATE::EXPLOSION_CHARGE;
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mGameObject->GetWorldPosition());

		mExplosionPrestartVFX->SetEnabled(true);
	}
}

void ExplosiveTrap::DoDamage()
{
	GameObject* playerObject = GameManager::GetInstance()->GetPlayer();
	PlayerController* player = GameManager::GetInstance()->GetPlayerController();
	float explosionRadius = mExplosionArea / 2.0f;
	float explosionRadiusSq = explosionRadius * explosionRadius;

	float distanceSq = mGameObject->GetWorldPosition().DistanceSq(playerObject->GetWorldPosition());
	if (distanceSq < explosionRadiusSq)
	{
		player->TakeDamage(mDamageAmount);
	}

	const std::vector<GameObject*>& allEnemies = App->GetScene()->FindGameObjectsWithTag("Enemy");
	for (GameObject* enemy : allEnemies)
	{
		if (enemy->IsEnabled())
		{
			if (mGameObject->GetWorldPosition().DistanceSq(enemy->GetWorldPosition()) <= explosionRadiusSq)
			{
				Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
				enemyScript->TakeDamage(mDamageAmount);
			}
		}
	}
}
