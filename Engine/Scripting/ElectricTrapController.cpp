#include "ElectricTrapController.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ParticleSystemComponent.h"

CREATE(ElectricTrapController)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mElectricVFX);
    END_CREATE;
}

ElectricTrapController::ElectricTrapController(GameObject* owner)
    : Script(owner), mActivationInterval(8.0f), mActivationDuration(4.0f)
{
}

ElectricTrapController::~ElectricTrapController()
{
    mInTrap.clear();
}

void ElectricTrapController::Start()
{
    GameObject* firstChild = *(mGameObject->GetChildren().begin());

    if (mElectricVFX)
    {
        mElectricVFX->SetEnabled(false);  // Initially disabled
    }

    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->SetSize(float3(mArea, 1.0f, mArea));
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ElectricTrapController::OnCollisionEnter, this, std::placeholders::_1)));
    }

    mState = TRAP_STATE::INACTIVE;
}

void ElectricTrapController::Update()
{
    switch (mState)
    {
    case TRAP_STATE::INACTIVE:
        if (mActivationIntervalTimer.Delay(mActivationInterval))
        {
            mState = TRAP_STATE::ACTIVE;
            ActiveTrap(true); 
            mActivationDurationTimer.Reset();
        }
        break;

    case TRAP_STATE::ACTIVE:
        if (mActivationDurationTimer.Delay(mActivationDuration))
        {
            mState = TRAP_STATE::INACTIVE;
            ActiveTrap(false);  
            mActivationIntervalTimer.Reset();  
        }
        break;
    }
}

bool ElectricTrapController::IsInTrap(const GameObject* target)
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

void ElectricTrapController::ActiveTrap(bool active)
{
    if (active)
    {
        if (mElectricVFX)
        {
            mElectricVFX->SetEnabled(true);
        }
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ELECTRICAL_TRAP, mGameObject->GetWorldPosition());
    }
    else
    {
        mInTrap.clear();
        if (mElectricVFX)
        {
            mElectricVFX->SetEnabled(false);
        }
    }
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;

    if (mState == TRAP_STATE::ACTIVE && !IsInTrap(collision))
    {
        mInTrap.push_back(collision);

        if (collision->GetTag().compare("Player") == 0)
        {
            PlayerController* player = GameManager::GetInstance()->GetPlayerController();
            player->Paralyzed(mSpeedReduction, true);  // Reduce speed to 70%
            player->TakeDamage(mDamageAmount);
        }

        if (collision->GetTag().compare("Enemy") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->Paralyzed(mSpeedReduction, true);  // Reduce speed to 70%
            enemy->TakeDamage(mDamageAmount);
        }
    }
}
