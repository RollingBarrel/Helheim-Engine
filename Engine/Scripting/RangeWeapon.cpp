#include "RangeWeapon.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "TrailComponent.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "PlayerController.h"
#include "RayCastBullet.h"


#include <vector>
#include <string>

RangeWeapon::RangeWeapon() : Weapon()
{
	mType = WeaponType::RANGE;
}

RangeWeapon::~RangeWeapon()
{
}

void RangeWeapon::Shoot(const float3& position, const float3& direction, const ColorGradient& trailGradient)
{
	Hit hit;

	Ray ray;
	ray.pos = position;
	ray.dir = direction;

	std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop" };
	Physics::Raycast(hit, ray, mAttackRange, &ignoreTags);

	if (GameManager::GetInstance()->GetPoolManager())
	{
		GameObject* bullet = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::BULLET);
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

