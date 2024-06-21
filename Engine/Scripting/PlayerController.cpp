#include "PlayerController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
#include "Physics.h"

#include "AudioSourceComponent.h"
#include "BoxColliderComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"

#include "Keys.h"
#include "Math/MathFunc.h"
#include "Geometry/Plane.h"
#include <functional>

#include "GameManager.h"
#include "HudController.h"

#include "State.h"
#include "MoveState.h"
#include "IdleState.h"
#include "DashState.h"
#include "AimState.h"
#include "AttackState.h"
#include "GrenadeState.h"
#include "SwitchState.h"
#include "SpecialState.h"
#include "ReloadState.h"

#include "Weapon.h"
#include "MeleeWeapon.h"
#include "RangeWeapon.h"
#include "Bat.h"
#include "Katana.h"
#include "Hammer.h"
#include "Pistol.h"
#include "Machinegun.h"
#include "Shootgun.h"
#include "Grenade.h"

CREATE(PlayerController)
{
    CLASS(owner);

    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxShield);
    MEMBER(MemberType::FLOAT, mPlayerSpeed);

    SEPARATOR("DASH");
    MEMBER(MemberType::FLOAT, mDashRange);
    MEMBER(MemberType::FLOAT, mDashCoolDown);
    MEMBER(MemberType::FLOAT, mDashDuration);

    SEPARATOR("MELEE");
    MEMBER(MemberType::GAMEOBJECT, mMeleeCollider);

    SEPARATOR("Grenade");
    MEMBER(MemberType::GAMEOBJECT, mGrenadeGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeAimAreaGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeExplotionPreviewAreaGO);
    MEMBER(MemberType::FLOAT, mGrenadeRange);
    MEMBER(MemberType::FLOAT, mGrenadeCoolDown);

    SEPARATOR("DEBUG MODE");
    MEMBER(MemberType::BOOL, mGodMode);

    END_CREATE;
}

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{
    mLowerStateType = StateType::NONE;
    mUpperStateType = StateType::NONE;
}

PlayerController::~PlayerController()
{
    delete mDashState;
    delete mIdleState;
    delete mMoveState;
    delete mAimState;
    delete mAttackState;
    delete mGrenadeState;
    delete mSwitchState;
    delete mSpecialState;
    delete mReloadState;

    delete mPistol;
    delete mBat;
    delete mMachinegun;
    delete mShootgun;
    delete mKatana;
    delete mHammer;
}

#pragma region MyRegion

void PlayerController::Start()
{
    // States
    mDashState = new DashState(this);
    mIdleState = new IdleState(this);
    mMoveState = new MoveState(this);
    mAimState = new AimState(this);
    mAttackState = new AttackState(this);
    mGrenadeState = new GrenadeState(this);
    mSwitchState = new SwitchState(this);
    mSpecialState = new SpecialState(this);
    mReloadState = new ReloadState(this);

    mLowerStateType = StateType::IDLE;
    mUpperStateType = StateType::AIM;
    mUpperState = mAimState;
    mLowerState = mIdleState;

    // Weapons
    BoxColliderComponent* collider = nullptr;
    if (mMeleeCollider) 
    {
        collider = reinterpret_cast<BoxColliderComponent*>(mMeleeCollider->GetComponent(ComponentType::BOXCOLLIDER));
        if (collider)
        {
            collider->AddCollisionEventHandler(
                CollisionEventType::ON_COLLISION_ENTER,
                new std::function<void(CollisionData*)>(std::bind(&Bat::OnCollisionEnter, (Bat*)mBat, std::placeholders::_1))
            );
        }
    }

    mBat = new Bat(collider);
    mPistol = new Pistol();
    mMachinegun = new Machinegun();
    mShootgun = new Shootgun();
    mKatana = new Katana();
    mHammer = new Hammer();

    mWeapon = mPistol;
    mSpecialWeapon = nullptr;

    // COLLIDER
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&PlayerController::OnCollisionEnter, this, std::placeholders::_1)));
    }

    // CAMERA
    mCamera = App->GetCamera()->GetCurrentCamera()->GetOwner();

    // GRENADE
    if (mGrenadeGO)
    {
        ScriptComponent* script = (ScriptComponent*)mGrenadeGO->GetComponent(ComponentType::SCRIPT);
        mGrenade = (Grenade*)script->GetScriptInstance();
        mGrenadeGO->SetEnabled(false);
        if (mGrenadeAimAreaGO) mGrenadeAimAreaGO->SetEnabled(false);
        if (mGrenadeExplotionPreviewAreaGO) mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    }

    // ANIMATION
    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }

}

void PlayerController::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    // Check input
    CheckInput();

    // Check state
    StateMachine();

    // Rotate the player to mouse
    HandleRotation();

    CheckDebugOptions();

    // Provisional
    UpdateBattleSituation();

}

void PlayerController::StateMachine()
{
    // Check if dead
    mLowerState->Update();
    mUpperState->Update();
}

void PlayerController::CheckInput() 
{
    // Lowerbody state machine
    StateType type = mLowerState->HandleInput();
    if (mLowerStateType != type) 
    {
        LOG(("LOWER: " + std::to_string(type)).c_str());
        mLowerStateType = type;
        mLowerState->Exit();

        switch (type) 
        {
            case StateType::DASH:
                mLowerState = mDashState;
                break;
            case StateType::MOVE:
                mLowerState = mMoveState;
                break;
            case StateType::IDLE:
                mLowerState = mIdleState;
                break;
            case StateType::NONE:
                break;
            default:
                break;
        }

        mLowerState->Enter();
    }
    

    // Upperbody state machine
    type = mUpperState->HandleInput();
    if (mUpperStateType != type)
    {
        LOG(("UPPER: " + std::to_string(type)).c_str());
        mUpperStateType = type;
        mUpperState->Exit();

        switch (type) 
        {
            case StateType::AIM:
                mUpperState = mAimState;
                break;
            case StateType::ATTACK:
                mUpperState = mAttackState;
                break;
            case StateType::GRENADE:
                mUpperState = mGrenadeState;
                break;
            case StateType::SWITCH:
                mUpperState = mSwitchState;
                break;
            case StateType::SPECIAL:
                mUpperState = mSpecialState;
                break;
            case StateType::RELOAD:
                mUpperState = mReloadState;
                break;
            case StateType::NONE:
                break;
            default:
                break;
        }

        mUpperState->Enter();
    }
}

void PlayerController::HandleRotation()
{
    // TODO: Not aim on melee state? and dash?

    GameManager* gameManager = GameManager::GetInstance();
    bool controller = gameManager->UsingController();

    if (controller)
    {
        float rightX = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
        float rightY = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);

        if (Abs(rightX) < 0.1f && Abs(rightY) < 0.1f) return;

        float3 position = mGameObject->GetPosition();
        mAimPosition.x = position.x - rightX;
        mAimPosition.y = position.y;
        mAimPosition.z = position.z - rightY;
    }
    else
    {
        Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
        Plane plane(mGameObject->GetPosition(), float3::unitY);

        float distance;
        if (plane.Intersects(ray, &distance))
        {
            mAimPosition = ray.GetPoint(distance);
        }
    }
    
    mGameObject->LookAt(mAimPosition);
}

void PlayerController::SetAnimation(std::string trigger, float transitionTime)
{
    if (mAnimationComponent) 
    {
        mAnimationComponent->SendTrigger(trigger, transitionTime);
    }
    
}

void PlayerController::SetSpineAnimation(std::string trigger, float transitionTime)
{
    if (mAnimationComponent) 
    {
        mAnimationComponent->SendSpineTrigger(trigger, transitionTime);
    }
    
}

void PlayerController::MoveInDirection(float3 direction)
{
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetDt() * mPlayerSpeed);
    mPlayerDirection = direction;
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(1.0f)));
}

void PlayerController::MoveToPosition(float3 position)
{
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(position, float3(10.0f)));
}

void PlayerController::SwitchWeapon() 
{
    mWeapon->Exit();
    if (mWeapon->GetType() == Weapon::WeaponType::MELEE) 
    {
        mWeapon = mPistol;

        switch (mEnergyType) 
        {
        case EnergyType::BLUE:
            mSpecialWeapon = mMachinegun;
            break;
        case EnergyType::RED:
            mSpecialWeapon = mShootgun;
            break;
        case EnergyType::NONE:
            mSpecialWeapon = nullptr;
            break;
        }
    }
    else 
    {
        mWeapon = mBat;

        switch (mEnergyType)
        {
        case EnergyType::BLUE:
            mSpecialWeapon = mKatana;
            break;
        case EnergyType::RED:
            mSpecialWeapon = mHammer;
            break;
        case EnergyType::NONE:
            mSpecialWeapon = nullptr;
            break;
        }
    }
    mWeapon->Enter();
}

float3 PlayerController::GetPlayerPosition()
{
    return  mGameObject->GetPosition(); 
}

void PlayerController::SetWeaponDamage(float percentage)
{
    mWeapon->SetDamage(mWeapon->GetDamage() * percentage);
}

void PlayerController::SetMaxShield(float percentage)
{
    mMaxShield *= percentage; 
    GameManager::GetInstance()->GetHud()->SetMaxHealth(mMaxShield);
}

void PlayerController::SetGrenadeVisuals(bool value)
{
    mGrenadeAimAreaGO->SetEnabled(value);
    mGrenadeAimAreaGO->SetScale(float3(mGrenadeRange, 0.5, mGrenadeRange));

    mGrenadeExplotionPreviewAreaGO->SetEnabled(value);
    mGrenadeExplotionPreviewAreaGO->SetScale(float3(mGrenade->GetGrenadeRadius(), 0.5f, mGrenade->GetGrenadeRadius()));
}

void PlayerController::UpdateGrenadeVisuals()
{
    mGrenadeAimAreaGO->SetPosition(mGameObject->GetPosition());

    float3 diff;
    if (GameManager::GetInstance()->UsingController())
    {
        mGrenadePosition = mGameObject->GetPosition() + mAimPosition * mGrenadeRange;
    }
    else
    {
        diff = mAimPosition - mGameObject->GetPosition();
        float distanceSquared = diff.LengthSq();
        float radiusSquared = mGrenadeRange * mGrenadeRange;

        if (distanceSquared <= radiusSquared)
        {
            mGrenadePosition = mAimPosition;
        }
        else 
        {
            diff.Normalize();
            mGrenadePosition = mGameObject->GetPosition() + diff * mGrenadeRange;
        }
    }
    
    mGrenadeExplotionPreviewAreaGO->SetPosition(float3(mGrenadePosition.x, 0.3f, mGrenadePosition.z));
}

void PlayerController::ThrowGrenade()
{
    // TODO wait for thow animation time
    mGrenade->SetDestination(mGrenadePosition);
}

bool PlayerController::CanReload() const
{
    if (mWeapon->GetCurrentAmmo() == mWeapon->GetMaxAmmo()) return false;

    return true;
}

void PlayerController::Reload() const
{
    mWeapon->SetCurrentAmmo(mWeapon->GetMaxAmmo());
    GameManager::GetInstance()->GetHud()->SetAmmo(mWeapon->GetCurrentAmmo());
}

void PlayerController::CheckDebugOptions()
{
    if (App->GetInput()->GetKey(Keys::Keys_G) == KeyState::KEY_REPEAT)
    {
        mGodMode = !mGodMode;
    }
}

void PlayerController::RechargeShield(float shield)
{
    if (mShield < mMaxShield) 
    {
        mShield = Clamp(mShield + shield, 0.0f, mMaxShield);

        float healthRatio = mShield / mMaxShield;
        GameManager::GetInstance()->GetHud()->SetHealth(healthRatio);
    }
}

void PlayerController::RechargeBattery(EnergyType batteryType)
{
    mCurrentEnergy = 100.0f;
    mEnergyType = batteryType;

    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType);

    switch (mEnergyType)
    {
        case EnergyType::NONE:
            break;
        case EnergyType::BLUE:
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                mSpecialWeapon = mMachinegun;
            }
            else
            {
                mSpecialWeapon = mKatana;
            }
            break;
        case EnergyType::RED:
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                mSpecialWeapon = mShootgun;
            }
            else
            {
                mSpecialWeapon = mHammer;
            }
            break;
        default:
            break;
    }
}

void PlayerController::UseEnergy(int energy)
{
    mCurrentEnergy -= energy;

    if (mCurrentEnergy <= 0)
    {
        mCurrentEnergy = 0;
        mEnergyType = EnergyType::NONE;
        mSpecialWeapon = nullptr;
    }
        
    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType);
}

void PlayerController::TakeDamage(float damage)
{
    if (mLowerState->GetType() == StateType::DASH || mGodMode)
    {
        return;
    }

    mShield = Clamp(mShield - damage, 0.0f, mMaxShield);

    float healthRatio = mShield / mMaxShield;
    GameManager::GetInstance()->GetHud()->SetHealth(healthRatio);

    if (mShield < 0.0f)
    {
        GameManager::GetInstance()->GameOver();
    }
}

void PlayerController::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetName() == "WinArea")
    {
        GameManager::GetInstance()->Victory();
    }
}

// PROVISIONAL
void PlayerController::UpdateBattleSituation()
{
    float hpRate = mShield / mMaxShield;

    /*if (mCurrentState == PlayerState::DEATH)
    {
        mCurrentSituation = BattleSituation::DEATH;
    }
    else if ((mPreviousState != PlayerState::ATTACK && mPreviousState != PlayerState::MOVE_ATTACK) &&
        (mCurrentState != PlayerState::ATTACK && mCurrentState != PlayerState::MOVE_ATTACK))
    {
        mBattleStateTransitionTime += App->GetDt();
        if (mBattleStateTransitionTime >= 8.0f)
        {
            if (hpRate <= 0.3)
            {
                mCurrentSituation = BattleSituation::IDLE_LOW_HP;
            }
            else
            {
                mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
            }
        }
    }
    else
    {
        mBattleStateTransitionTime = 0.0f;

        if (hpRate <= 0.3)
        {
            mCurrentSituation = BattleSituation::BATTLE_LOW_HP;
        }
        else {
            mCurrentSituation = BattleSituation::BATTLE_HIGHT_HP;
        }
    }*/
}