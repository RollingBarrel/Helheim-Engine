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

void EnemyExplosiveTrap::Start()
{
	Enemy::Start();
	ScriptComponent* script = static_cast<ScriptComponent*>(mOuterTrap->GetComponent(ComponentType::SCRIPT));
	mTrap = static_cast<ExplosiveTrap*>(script->GetScriptInstance());
}

void EnemyExplosiveTrap::Update() 
{
}

void EnemyExplosiveTrap::TakeDamage(float damage)
{
	mTrap->InnerTrapTakeDamage();
}
