#include "ExplosiveTrap.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ParticleSystemComponent.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "PlayerController.h"
#include "AudioManager.h"

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


ExplosiveTrap::ExplosiveTrap(GameObject* owner) : Script(owner)
{

}

ExplosiveTrap::~ExplosiveTrap()
{
    mInTrap.clear();
}

void ExplosiveTrap::Start()
{
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        float3 area(mTriggerArea, 1, mTriggerArea);
        mCollider->SetSize(area);
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ExplosiveTrap::OnCollisionEnter, this, std::placeholders::_1)));
    }

    if (mExplosionPrestartVFX != nullptr)
    {
        float3 area(mExplosionArea + 2, mExplosionArea + 2, mExplosionArea + 2);
        mExplosionPrestartVFX->SetLocalScale(area);
    }

    if (mExplosionVFX != nullptr)
    {
        float3 area(mExplosionArea, mExplosionArea, mExplosionArea);
        mExplosionVFX->SetLocalScale(area);
    }
}

void ExplosiveTrap::Update()
{
    if (mState == TRAP_STATE::EXPLOSION_PRESTART)
    {
        if (mExplosionWaitTimer.Delay(mExplosionWait))
        {
            mState = TRAP_STATE::EXPLOSION_START;
            mExplosionPrestartVFX->SetEnabled(false);
            mExplosionVFX->SetEnabled(true);
            mBarrelMesh->SetEnabled(false);

            GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_EXPLOSION, mGameObject->GetWorldPosition());

            // Change box size to hit farther 
            float3 area(mExplosionArea/2, 1, mExplosionArea/2);
            mCollider->SetSize(area);
        }
    }

    if (mState == TRAP_STATE::EXPLOSION_START)
    {
        if (mExplosionWaitTimer.Delay(mExplosionDuration))
        {
            mGameObject->SetEnabled(false);
        }
    }
}


bool ExplosiveTrap::IsInTrap(const GameObject* target)
{
    for (auto captured : mInTrap)
    {
        if (captured->GetID() == target->GetID())
        {
            return true;
        }
    }
    return false;
}

void ExplosiveTrap::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;

    if (mState == TRAP_STATE::INACTIVE)
    {
        if (collision->GetTag().compare("Player") == 0)
        {
            mState = TRAP_STATE::EXPLOSION_PRESTART;
            GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mGameObject->GetWorldPosition());

            mExplosionPrestartVFX->SetEnabled(true);
        }
    }

    if (mState == TRAP_STATE::EXPLOSION_START && !IsInTrap(collision))
    {
        mInTrap.push_back(collision);

        if (collision->GetTag().compare("Player") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            PlayerController* player = static_cast<PlayerController*>(script->GetScriptInstance());
            player->TakeDamage(mDamageAmount);
        }

        if (collision->GetTag().compare("Enemy") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->TakeDamage(mDamageAmount);
        }
    }
}

void ExplosiveTrap::InnerTrapTakeDamage()
{
    if (mState != TRAP_STATE::EXPLOSION_START)
    {
        mState = TRAP_STATE::EXPLOSION_PRESTART;
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mGameObject->GetWorldPosition());

        mExplosionPrestartVFX->SetEnabled(true);
    }
}
