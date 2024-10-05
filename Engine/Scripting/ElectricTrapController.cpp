#include "ElectricTrapController.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ScriptComponent.h"

CREATE(ElectricTrapController)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT, mWarningDuration);
    MEMBER(MemberType::FLOAT, mActiveDuration);
    MEMBER(MemberType::FLOAT, mDamageAmount);
    MEMBER(MemberType::FLOAT, mSpeedReduction);
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
    mWarningTimer.Reset();
    mActiveTimer.Reset();
    // Reuse the WarningVFX for both WARNING and ACTIVE states
    mWarningVFX = App->GetScene()->InstantiatePrefab("TrapSFX.prfb");

    if (mWarningVFX)
    {
        mWarningVFX->SetEnabled(false);
        mWarningVFX->SetParent(mGameObject);
    }

    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER,
            new std::function<void(CollisionData*)>(std::bind(&ElectricTrapController::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void ElectricTrapController::Update()
{
    // Get the distance between the player and the trap
    float distance = GameManager::GetInstance()->GetPlayer()->GetWorldPosition().Distance(mGameObject->GetWorldPosition());

    // Proceed with state transitions regardless of distance once triggered
    switch (mState)
    {
    case TRAP_STATE::INACTIVE:
       // if (distance <= 20) // Only trigger this once when the player gets close
      //  {
            if (mWarningTimer.Delay(2.0f))  // Inactive phase duration
            {
                LOG("Changing state from INACTIVE to WARNING");
                SetTrapState(TRAP_STATE::WARNING);
            }
     //   }
        break;

    case TRAP_STATE::WARNING:
        LOG(("Warning Timer Value: " + std::to_string(mWarningTimer.GetTimePassed())).c_str());

        // Change to ACTIVE after the warning duration (e.g., 1 second)
        if (mWarningTimer.Delay(mWarningDuration))
        {
            LOG("Changing state from WARNING to ACTIVE");
            SetTrapState(TRAP_STATE::ACTIVE);
        }
        break;

    case TRAP_STATE::ACTIVE:
        LOG(("Active Timer Value: " + std::to_string(mActiveTimer.GetTimePassed())).c_str());

        // Change to INACTIVE after the active duration (e.g., 4 seconds)
        if (mActiveTimer.Delay(mActiveDuration))
        {
            LOG("Changing state from ACTIVE to INACTIVE");
            SetTrapState(TRAP_STATE::INACTIVE);
        }
        break;
    }
}



void ElectricTrapController::ActivateWarningVFX(bool active)
{
    if (mWarningVFX)
    {
        mWarningVFX->SetEnabled(active);
    }
}

bool ElectricTrapController::IsInTrap(const GameObject* target)
{
    for (auto& obj : mInTrap)
    {
        if (obj->GetID() == target->GetID()) return true;
    }
    return false;
}

void ElectricTrapController::SetTrapState(TRAP_STATE state)
{
    switch (state)
    {
    case TRAP_STATE::INACTIVE:
        LOG("Trap State: INACTIVE");
        ActivateWarningVFX(false);
        mInTrap.clear();  // Clear captured targets
        mState = TRAP_STATE::INACTIVE;
        mWarningTimer.Reset();  // Reset timer for the next cycle
        mActiveTimer.Reset();
        break;

    case TRAP_STATE::WARNING:
        ActivateWarningVFX(true);  // Activate VFX at the start of WARNING state
        LOG("Trap State: WARNING");
        mWarningTimer.Reset();  // Start warning timer
        mState = TRAP_STATE::WARNING;
        break;

    case TRAP_STATE::ACTIVE:
        LOG("Trap State: ACTIVE");
        mActiveTimer.Reset();  // Start active timer
        mState = TRAP_STATE::ACTIVE;
        break;
    }
}


void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collidedObject = collisionData->collidedWith;
   // LOG(("Active Duration: " + std::to_string(mActiveDuration)).c_str()); // Ensure you log the correct duration

   // LOG(("Collision detected with: " + collidedObject->GetTag()).c_str());
    if (mState == TRAP_STATE::ACTIVE) {
        LOG("aactive")
    }   
    if (mState == TRAP_STATE::WARNING) {
            LOG("wariniiiing")
    }

    if (!IsInTrap(collidedObject) && mState == TRAP_STATE::ACTIVE)
    {
        LOG(("Entering trap with: " + collidedObject->GetTag()).c_str());

        mInTrap.push_back(collidedObject);
        if (collidedObject->GetTag() == "Player")
        {
            PlayerController* player = GameManager::GetInstance()->GetPlayerController();
            player->TakeDamage(mDamageAmount);
            player->Paralyzed(mSpeedReduction, true);
            LOG(("Player took damage: " + std::to_string(mDamageAmount)).c_str());
        }
        else if (collidedObject->GetTag() == "Enemy")
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collidedObject->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->TakeDamage(mDamageAmount);
            enemy->Paralyzed(mSpeedReduction, true);
            LOG(("Enemy took damage: " + std::to_string(mDamageAmount)).c_str());
        }
    }
}
