#include "RangeWeapon.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "RayCastBullet.h"

#include <vector>
#include <string>
#include <random>

RangeWeapon::RangeWeapon() : Weapon()
{
	mType = WeaponType::RANGE;
}

void RangeWeapon::Shoot(const float3& position, float maxSpread, const ColorGradient& trailGradient)
{
	GameManager::GetInstance()->GetPlayerCamera()->ActivateShake(mCameraShakeDuration, mCameraShakeStrengh);

	//float3 front = GameManager::GetInstance()->GetPlayer()->GetFront();
	float3 front = GameManager::GetInstance()->GetPlayerController()->GetPlayerAimPosition() - GameManager::GetInstance()->GetPlayerController()->GetShootOriginGO()->GetWorldPosition();
	front.y = 0.0f;
	front.Normalize();
	float3 up = GameManager::GetInstance()->GetPlayer()->GetUp();
	float3 right = GameManager::GetInstance()->GetPlayer()->GetRight();
	float3 bulletDirection = Spread(front, up, right, maxSpread);

	Hit hit;

	Ray ray;
	ray.pos = position;
	ray.dir = bulletDirection;

	std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop" };
	Physics::Raycast(hit, ray, mAttackRange, &ignoreTags);

	if (GameManager::GetInstance()->GetPoolManager())
	{
		GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
		//This line is causing the game to slow down because for some reason it makes the bullets
		// constantly collide with an enemy activating every frame the enemy hitstop and the hitsound 
		//bullet->SetWorldRotation(GameManager::GetInstance()->GetPlayer()->GetWorldRotation());
		RayCastBullet* bulletScript = reinterpret_cast<RayCastBullet*>(reinterpret_cast<ScriptComponent*>(bullet->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		bullet->SetEnabled(false);

		if (hit.IsValid())
		{
			bulletScript->Init(ray.pos, hit, mDamage, mBulletSpeed, mBulletSize, &trailGradient);
		}
		else
		{
			bulletScript->Init(ray.pos, ray.pos + ray.dir.Mul(mAttackRange), mDamage, mBulletSpeed, mBulletSize, &trailGradient);
		}
	}
}

float3 RangeWeapon::Spread(const float3& front, const float3& up, const float3& right, float maxSpread)
{

	std::random_device rdev;
	std::mt19937 rgen(rdev());
	std::uniform_real_distribution<float> idist(-maxSpread, maxSpread);
	
	float upSpread = idist(rgen);
	float rightSpread = idist(rgen);

	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = 0.2f;
	float r = random * diff;

	//float random = ((float)rand()) / (float)RAND_MAX;
	//float diff = maxSpread - (-maxSpread);
	//float upSpread = random * diff;
	//random = ((float)rand()) / (float)RAND_MAX;
	//float rightSpread = random * diff;
	//random = ((float)rand()) / (float)RAND_MAX;
	//diff = 0.2f;
	//float r = random * diff;

	float3 spread = float3::zero;
	spread += up * upSpread;
	spread += right * rightSpread;
	spread *= r;

	return (front + spread).Normalized();
}

