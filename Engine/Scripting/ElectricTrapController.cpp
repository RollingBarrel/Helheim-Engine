#include "ElectricTrapController.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"
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

    mPlayer = GameManager::GetInstance()->GetPlayerController();

    mDamageTimer = mDamageDelay;

    ActivateTrap(false, false);
}

void ElectricTrapController::Update()
{
    if (!GameManager::GetInstance()->IsPaused())
    {

        float distance = GameManager::GetInstance()->GetPlayer()->GetWorldPosition().Distance(mGameObject->GetWorldPosition());
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
                
                    if (mActivationIntervalTimer.Delay(mFirstActivationInterval) && mIsAwake)
                    {
                        mIsActive = true;
                        ActivateTrap(true, false);  // Trap active, VFX off
                        mFirstActivation = false;
                    }
                }
               
                if (mActivationIntervalTimer.Delay(mActivationInterval) && mIsAwake)
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
    else
    {
        mIsActive = false;
    }
}

void ElectricTrapController::ActivateTrap(bool active, bool vfxOnly)
{
    if (active)
    {
        if (mSfx)
        {
            GameManager::GetInstance()->GetAudio()->PlayOneShot(mTrapSound, mGameObject->GetWorldPosition());

            mSfx->SetEnabled(true);
        }
    }
    else
    {
        if (mSfx)
        {
            mSfx->SetEnabled(false);
        }
    }
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;

    if (mIsActive)
    {
        if (collision->GetTag().compare("Player") == 0)
        {
            if (mDamageTimer < mDamageDelay)
            {
                mDamageTimer += App->GetDt();
            }
            else
            {
                mDamageTimer = 0;

                GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ELECTRICAL_TRAP1, mGameObject->GetWorldPosition());
 
                mPlayer = GameManager::GetInstance()->GetPlayerController();
                mPlayer->Paralyzed(mSpeedReduction, true);
                mPlayer->TakeDamage(mDamageAmount);

            }
        }

        if (collision->GetTag().compare("Enemy") == 0 && mIsActive)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->Paralyzed(mSpeedReduction, true);
        }
    }
}
