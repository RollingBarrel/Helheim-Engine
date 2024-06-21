#include "Shootgun.h"
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

Shootgun::Shootgun()
{
    mDamage = 0.1f;
    mAttackRange = 100.0f;
    mAttackDuration = 1.0f;
}

Shootgun::~Shootgun()
{
}

void Shootgun::Enter()
{
   
}

void Shootgun::Attack(float time)
{
    int numBullets = 10;

    //TODO: Rethink this
    reinterpret_cast<PlayerController*>(reinterpret_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT))->GetScriptInstance())->UseEnergy(numBullets);
   
    //Audio
    if (GameManager::GetInstance()->GetAudio())
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::GUNFIRE, GameManager::GetInstance()->GetPlayer()->GetPosition());
    }

    //Shoot Logic
    int count = 0;
    for (int i = 0; i < numBullets; ++i)
    {
        Ray ray;
        ray.pos = GameManager::GetInstance()->GetPlayer()->GetPosition();
        ray.pos.y++;
        ray.dir = GameManager::GetInstance()->GetPlayer()->GetFront();

        float3 spread = float3::zero;
        spread += GameManager::GetInstance()->GetPlayer()->GetUp() * LCG().Float(-1.0f, 1.0f);
        spread += GameManager::GetInstance()->GetPlayer()->GetRight() * LCG().Float(-1.0f, 1.0f);

        ray.dir += spread.Normalized() * LCG().Float(0.0f, 0.2f);


        Hit hit;
        Physics::Raycast(hit, ray, mAttackRange);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag() == "Enemy")
            {
                LOG("Enemy %s has been hit with ShotGun at distance: %f", hit.mGameObject->GetName().c_str(), hit.mDistance);
                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(mDamage);
                }
            }
        }
        else
        {
            count++;
        }
    }
    
    LOG("Missed bullets = %i", count);
}

void Shootgun::Exit()
{
}

void Shootgun::Reload()
{
    mCurrentAmmo = mMaxAmmo;
    GameManager::GetInstance()->GetHud()->SetAmmo(mCurrentAmmo);
}
