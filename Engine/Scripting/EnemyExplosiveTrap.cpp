#include "EnemyExplosiveTrap.h"
#include "ExplosiveTrap.h"
#include "GameObject.h"
#include "ScriptComponent.h"

CREATE(EnemyExplosiveTrap)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mOuterTrap);
    END_CREATE;
}

void EnemyExplosiveTrap::Update()
{
	// Do nothing as it is just a trap, but CAN BE HIT as enemies
}

void EnemyExplosiveTrap::TakeDamage(float damage)
{
	// Do not receive damages
	if (!mIsActive)
	{
		const ScriptComponent* script = static_cast<ScriptComponent*>(mOuterTrap->GetComponent(ComponentType::SCRIPT));
		ExplosiveTrap* OuterTrap = static_cast<ExplosiveTrap*>(script->GetScriptInstance());
		OuterTrap->InnerTrapTakeDamage();

		mIsActive = true;
	}

}
