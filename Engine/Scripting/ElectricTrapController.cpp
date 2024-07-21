#include "ElectricTrapController.h"
#include "GameManager.h"
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
    SEPARATOR("ACTIVATION PARAMETER");
    MEMBER(MemberType::FLOAT, mActivationInterval);
    MEMBER(MemberType::FLOAT, mActivationDuration);
    MEMBER(MemberType::BOOL, mIsActive);

    SEPARATOR("SIDE EFFECT");
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
    mSfx = App->GetScene()->InstantiatePrefab("TrapSFX.prfb");
    if (mSfx)
    {
        mSfx->SetParent(mGameObject);
        mSfx->SetEnabled(mIsActive);
    }

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&ElectricTrapController::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void ElectricTrapController::Update()
{
    // Update the timer
    mTimer -= App->GetDt();

    if (mIsActive)
    {
        if (mTimer <= 0.0f)
        {
            mTimer = mActivationInterval;
            mIsActive = false;
            DeactiveTrap();
        }
    }
    else
    {
        if (mTimer <= 0.0f)
        {
            mTimer = mActivationDuration;
            mIsActive = true;
            ActiveTrap();
        }
    }
}



bool ElectricTrapController::CheckIfCaptured(const GameObject* target)
{
    for (auto captured : mInTrap)
    {
        if (captured == target)
        {
            return true;
        }
    }
    return false;
}

void ElectricTrapController::DeactiveTrap()
{
    mInTrap.clear();
    LOG("Trap not active");
    if (mSfx)
    {
        mSfx->SetEnabled(false);
    }
    // Reserved for effects, perticle, sounds...

}

void ElectricTrapController::ActiveTrap()
{
    LOG("Trap active");
    if (mSfx)
    {
        mSfx->SetEnabled(true);
    }
    // Reserved for effects, perticle, sounds...
}

void ElectricTrapController::OnCollisionEnter(CollisionData* collisionData)
{
    GameObject* collision = collisionData->collidedWith;

    if (!CheckIfCaptured(collision) && mIsActive)
    {
        mInTrap.push_back(collision);

        if (collision->GetTag().compare("Player") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            PlayerController* player = static_cast<PlayerController*>(script->GetScriptInstance());
            player->GetParalyzed(mSpeedReduction);
            player->TakeDamage(mDamageAmount);
        }

        if (collision->GetTag().compare("Enemy") == 0)
        {
            const ScriptComponent* script = static_cast<ScriptComponent*>(collision->GetComponent(ComponentType::SCRIPT));
            Enemy* enemy = static_cast<Enemy*>(script->GetScriptInstance());
            enemy->GetParalyzed(mSpeedReduction);
            enemy->TakeDamage(mDamageAmount);
        }
    }
}
