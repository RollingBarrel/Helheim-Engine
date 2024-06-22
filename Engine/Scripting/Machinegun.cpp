#include "Machinegun.h"
#include "GameManager.h"
#include "HudController.h"
#include "AudioManager.h"
#include "Enemy.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Physics.h"

#include "Geometry/Ray.h"
#include "Algorithm/Random/LCG.h"
#include <PlayerController.h>

Machinegun::Machinegun()
{
	mAttackRange = 100.0f;
	mDamage = 0.4f;
	mAttackDuration = 0.1f;
	
	mCurrentAmmo = 32;
	mMaxAmmo = 32;
}

Machinegun::~Machinegun()
{
}

void Machinegun::Enter()
{
}

void Machinegun::Attack(float time)
{
    LOG("MachineGun Attack");
	//Audio
	if (GameManager::GetInstance()->GetAudio())
	{
		GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetPosition());
	}

	//Shoot Logic
    Ray ray;
    ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
    ray.pos.y++;
    ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

    //Bullets go straigh for now
    //ray.dir += spread.Normalized() * LCG().Float(0.0f, 0.2f);


    Hit hit;
    Physics::Raycast(hit, ray, mAttackRange);

    if (hit.IsValid())
    {
        if (hit.mGameObject->GetTag() == "Enemy")
        {
            LOG("Enemy %s has been hit with MachineGun at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
            Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
            if (enemy)
            {
                enemy->TakeDamage(mDamage);
            }
        }
        //DELETE BULLET?
        //Log position where bullet has to be destroyed and track bullet postion, modifying it with bulletSpeed until it reaches (surpasses) that position
    }

}

void Machinegun::Exit()
{
}

void Machinegun::Reload() 
{
    mCurrentAmmo = mMaxAmmo;
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}
