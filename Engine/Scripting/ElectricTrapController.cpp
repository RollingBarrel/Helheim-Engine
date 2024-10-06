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

CREATE(ElectricTrapController)
{
    CLASS(owner);
    MEMBER(MemberType::BOOL, mIsAwake);
    MEMBER(MemberType::BOOL, mIsActive);
    END_CREATE;
}

ElectricTrapController::ElectricTrapController(GameObject* owner) : Script(owner)
{
}

ElectricTrapController::~ElectricTrapController()
{
    mInTrap.clear();
}

void ElectricTrapController::Start()
{
    mSfx = App->GetScene()->InstantiatePrefab("TrapSFX.prfb");
    if (mSfx)
    {
        mSfx->SetParent(mGameObject);
        mSfx->SetEnabled(mIsActive);
    }

    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ElectricTrapController::OnCollisionEnter, this, std::placeholders::_1)));
    }
    ActivateTrap(false, false);
}

void ElectricTrapController::Update()
{
    float distance = GameManager::GetInstance()->GetPlayer()->GetWorldPosition().Distance(mGameObject->GetWorldPosition());
    LOG("Distance: %f", distance);
    if (distance <= 30)
    {
        if (mIsActive)
        {
            if (mActivationDurationTimer.Delay(mActivationDuration) && mIsAwake)
            {
                mIsActive = false;
                ActivateTrap(false, false);
            }
        }
        else
        {
            if (mFirstActivation)
            {
                if (mVFXWarningTimer.Delay(mFirstActivationInterval - mVFXWarningDuration) && !mVFXWarningTimer.Delay(mFirstActivationInterval) && mIsAwake)
                {
                    ActivateTrap(false, true);  // VFX only
                }

                if (mActivationIntervalTimer.Delay(mFirstActivationInterval) && mIsAwake)
                {
                    mIsActive = true;
                    ActivateTrap(true, false);  // Trap active, VFX off
                    mFirstActivation = false;
                }
            }
            else if (mVFXWarningTimer.Delay(mActivationInterval - mVFXWarningDuration) && !mVFXWarningTimer.Delay(mActivationInterval) && mIsAwake)
            {
                ActivateTrap(false, true);  // VFX only
            }
            else if (mActivationIntervalTimer.Delay(mActivationInterval) && mIsAwake)
            {
                mIsActive = true;
                ActivateTrap(true, false);  // Trap active, VFX off
            }
        }
    }
    else
    {
        mIsAwake = false;
        ActivateTrap(false, false);
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

void ElectricTrapController::ActivateTrap(bool active, bool vfxOnly)
{
    if (active)
    {
        if (mSfx)
        {
            mSfx->SetEnabled(true);
        }
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ELECTRICAL_TRAP, mGameObject->GetWorldPosition());
    }
    else if (vfxOnly)
    {
        if (mSfx)
        {
            mSfx->SetEnabled(true);  // Only play VFX, no sound or damage
        }
    }
    else
    {
        mInTrap.clear();
        if (mSfx)
        {
            mSfx->SetEnabled(false);
        }
    }
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;

    if (!IsInTrap(collision) && mIsActive)
    {
        mInTrap.push_back(collision);

        if (collision->GetTag().compare("Player") == 0)
        {
            PlayerController* player = GameManager::GetInstance()->GetPlayerController();
            player->Paralyzed(mSpeedReduction, true);
            player->TakeDamage(mDamageAmount);
        }

        if (collision->GetTag().compare("Enemy") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->Paralyzed(mSpeedReduction, true);
        }
    }
}
