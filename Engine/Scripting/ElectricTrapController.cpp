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
#include <EditorControlPanel.h>


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
        mSfx->SetEnabled(false);
    }

    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ElectricTrapController::OnCollisionEnter, this, std::placeholders::_1)));
    }
    ActiveTrap(false);
}

void ElectricTrapController::Update() {
    float distance = GameManager::GetInstance()->GetPlayer()->GetWorldPosition().Distance(mGameObject->GetWorldPosition());
    if (distance <= 20) {

        LOG("Player is close to the trap: %s", StateToString(mCurrentState));
        if (mIsActive) {
            if (mActivationDurationTimer.Delay(mActivationDuration)) {
                mIsActive = false;
                ActiveTrap(false);
            }
        }
        else if (mCurrentState == mState::WARNING) {
            if (mActivationDurationTimer.Delay(mActivationDuration)) {
                mCurrentState = mState::ACTIVE; // Change to active state after the warning duration
                ActiveTrap(true);
            }
        }
        else {
            if (mFirstActivation) {
                if (mActivationIntervalTimer.Delay(mFirstActivationInterval)) {
                    mCurrentState = mState::WARNING; // Change to warning state
                    ActiveTrap(true); // Enable VFX for warning
                    mFirstActivation = false;
                }
            }
            else if (mActivationIntervalTimer.Delay(mActivationInterval)) {
                mCurrentState = mState::WARNING; // Change to warning state
                ActiveTrap(true); // Enable VFX for warning
            }
        }
    }
    else {
        mIsAwake = false;
        ActiveTrap(false);
        mCurrentState = mState::INACTIVE; // Reset to inactive if the player is not close
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

void ElectricTrapController::ActiveTrap(bool active) {
    if (active) {
        if (mSfx) {
            mSfx->SetEnabled(true); // Enable VFX
        }
        if (mCurrentState == mState::ACTIVE) {
            GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ELECTRICAL_TRAP, mGameObject->GetWorldPosition());
        }
    }
    else {
        mInTrap.clear();
        if (mSfx) {
            mSfx->SetEnabled(false); // Disable VFX if inactive
        }
    }
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData) {
    GameObject* collision = collisionData->collidedWith;

    if (!IsInTrap(collision) && mCurrentState == mState::ACTIVE) { // Only apply damage if ACTIVE
        mInTrap.push_back(collision);

        if (collision->GetTag().compare("Player") == 0) {
            PlayerController* player = GameManager::GetInstance()->GetPlayerController();
            player->Paralyzed(mSpeedReduction, true);
            player->TakeDamage(mDamageAmount);
        }

        if (collision->GetTag().compare("Enemy") == 0) {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->Paralyzed(mSpeedReduction, true);
        }
    }
}

const char* ElectricTrapController::StateToString(mState state) {
    switch (state) {
    case mState::INACTIVE: return "INACTIVE";
    case mState::AWAKE: return "AWAKE";
    case mState::WARNING: return "WARNING";
    case mState::ACTIVE: return "ACTIVE";
    default: return "UNKNOWN";
    }
}
