#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "HudController.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Bat.h"
#include "TrailComponent.h"
#include "HudController.h"
#include "GameManager.h"
#include "AudioManager.h"

#include "ModuleInput.h"

#include <map>

Pistol::Pistol() : RangeWeapon()
{
	mCurrentAmmo = 16;
	mMaxAmmo = 16;
	mDamage = 2.0f;
	mAttackDuration = 0.0f;
	mAttackCooldown = 0.2f;

	mFire = App->GetScene()->InstantiatePrefab("PistolFire.prfb");
	if (mFire)
	{
		mFire->SetEnabled(false);
	}

}

Pistol::~Pistol()
{
}

void Pistol::Enter()
{
	//CONTROLLER VIBRATION
	App->GetInput()->SetGameControllerRumble(45000, 0, 100);
}

void Pistol::Attack(float time)
{
	// LOG("Pistol Attack");

	 //Audio
	if (GameManager::GetInstance()->GetAudio())
	{
		PlayHitSound();
	}

	GameObject* bullet = nullptr;
	if (mCurrentAmmo > 0)
	{
		mCurrentAmmo--;
		//LOG("Bullets: %i", mCurrentAmmo);
	}

	GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);

	Hit hit;

	Ray ray;
	ray.pos = GameManager::GetInstance()->GetPlayer()->GetWorldPosition();
	ray.pos.y++;
	ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

	float distance = 100.0f;
	Physics::Raycast(hit, ray, distance);

	if (hit.IsValid())
	{
		//LOG("Object %s has been hit at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
		if (hit.mGameObject->GetTag().compare("Enemy") == 0)
		{
			//LOG("Enemy %s has been hit at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);

			Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
			if (enemy)
			{
				enemy->TakeDamage(mDamage);
			}
		}

	}

	//PARTICLES
	if (mFire)
	{
		mFire->SetEnabled(false);
		mFire->SetEnabled(true);
		mFire->SetWorldPosition(ray.pos + GameManager::GetInstance()->GetPlayer()->GetFront());
	}

	if (GameManager::GetInstance()->GetPoolManager())
	{
		GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
		Bullet* bulletScript = reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		ColorGradient gradient;
		gradient.AddColorGradientMark(0.1f, float4(0.0f, 1.0f, 0.0f, 1.0f));
		bullet->SetEnabled(false);
		bulletScript->Init(ray.pos, ray.dir, 1.0f, 1.0f, &gradient);
	}
}

void Pistol::Reload()
{
	mCurrentAmmo = mMaxAmmo;
	GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}

void Pistol::PlayHitSound()
{
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
}

void Pistol::Exit()
{
}
