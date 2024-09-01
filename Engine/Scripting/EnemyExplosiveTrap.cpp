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
	// DO NOTHING
}

void EnemyExplosiveTrap::TakeDamage(float damage)
{
	// Don not receive damage
	if (!mIsActive)
	{
		const ScriptComponent* script = static_cast<ScriptComponent*>(mOuterTrap->GetComponent(ComponentType::SCRIPT));
		ExplosiveTrap* OuterTrap = static_cast<ExplosiveTrap*>(script->GetScriptInstance());
		OuterTrap->InnerTrapTakeDamage();

		mIsActive = true;
	}

}
