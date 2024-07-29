#include "Pistol.h"
#include "Application.h"

#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PoolManager.h"
#include "PlayerController.h"
#include "HudController.h"
#include "Enemy.h"
#include "RayCastBullet.h"


Pistol::Pistol() : RangeWeapon()
{
	mCurrentAmmo = 16;
	mMaxAmmo = 16;
	mDamage = 2.0f;
	mAttackDuration = 0.0f;
	mAttackCooldown = 0.15f;
	mAttackRange = 25.0f;

	mBulletSpeed = 50.0f;

	mFire = App->GetScene()->InstantiatePrefab("PistolFire.prfb");
	if (mFire)	mFire->SetEnabled(false);
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

	std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop"};
	Physics::Raycast(hit, ray, mAttackRange, &ignoreTags);

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
		RayCastBullet* bulletScript = reinterpret_cast<RayCastBullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		ColorGradient gradient;
		gradient.AddColorGradientMark(0.1f, float4(0.0f, 1.0f, 0.0f, 1.0f));
		bullet->SetEnabled(false);

		if (hit.IsValid())
		{
			bulletScript->Init(ray.pos, hit, mDamage, mBulletSpeed, mBulletSize, &gradient);
		}
		else
		{
			bulletScript->Init(ray.pos, ray.pos + ray.dir.Mul(mAttackRange), mDamage, mBulletSpeed, mBulletSize, &gradient);
		}
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
