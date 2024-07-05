#include "PlayerController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleDetourNavigation.h"
#include "ModuleResource.h"
#include "GameObject.h"
#include "Physics.h"

#include "AudioSourceComponent.h"
#include "BoxColliderComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"

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
    MEMBER(MemberType::GAMEOBJECT, mEquippedMeleeGO);
    MEMBER(MemberType::GAMEOBJECT, mUnEquippedMeleeGO);
    MEMBER(MemberType::GAMEOBJECT, mEquippedGunGO);
    MEMBER(MemberType::GAMEOBJECT, mUnEquippedGunGO);
    MEMBER(MemberType::GAMEOBJECT, mEquippedSpecialGO);
    MEMBER(MemberType::GAMEOBJECT, mUnEquippedSpecialGO);
    MEMBER(MemberType::GAMEOBJECT, mMeleeCollider);
    MEMBER(MemberType::GAMEOBJECT, mBatTrail);
    MEMBER(MemberType::GAMEOBJECT, mKatanaTrail);
    MEMBER(MemberType::GAMEOBJECT, mHammerTrail);

    SEPARATOR("Grenade");
    MEMBER(MemberType::GAMEOBJECT, mGrenadeGO);
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
    mDashState = new DashState(this, mDashCoolDown);
    mIdleState = new IdleState(this, 0.0f);
    mMoveState = new MoveState(this, 0.0f);
    mAimState = new AimState(this, 0.0f);
    mGrenadeState = new GrenadeState(this, mGrenadeCoolDown);
    mSwitchState = new SwitchState(this, mSwitchCoolDown);
    mAttackState = new AttackState(this, 0.0f); // Is later changed when having a weapon
    mSpecialState = new SpecialState(this, 0.0f); // Is later changed when having a weapon
    mReloadState = new ReloadState(this, 0.0f);

    mLowerStateType = StateType::IDLE;
    mUpperStateType = StateType::AIM;
    mUpperState = mAimState;
    mLowerState = mIdleState;


    // Weapons
    BoxColliderComponent* weaponCollider = nullptr;
    if (mMeleeCollider) 
    {
        weaponCollider = reinterpret_cast<BoxColliderComponent*>(mMeleeCollider->GetComponent(ComponentType::BOXCOLLIDER));
    }
    TrailComponent* batTrail = nullptr;
    if (mBatTrail)
    {
        batTrail = reinterpret_cast<TrailComponent*>(mBatTrail->GetComponent(ComponentType::TRAIL));
    }
    TrailComponent* katanaTrail = nullptr;
    if (mKatanaTrail)
    {
        katanaTrail = reinterpret_cast<TrailComponent*>(mKatanaTrail->GetComponent(ComponentType::TRAIL));
    }
    TrailComponent* hammerTrail = nullptr;
    if (mHammerTrail)
    {
        hammerTrail = reinterpret_cast<TrailComponent*>(mHammerTrail->GetComponent(ComponentType::TRAIL));
    }

    mBat = new Bat(weaponCollider, batTrail);
    mBat->Exit();
    mPistol = new Pistol();
    mMachinegun = new Machinegun();
    mShootgun = new Shootgun();
    mKatana = new Katana(weaponCollider, katanaTrail);
    mHammer = new Hammer(weaponCollider, hammerTrail);

    mWeapon = mPistol;
    mAttackState->SetCooldown(mWeapon->GetAttackCooldown());
    mSpecialWeapon = nullptr;

    if (mEquippedMeleeGO && mUnEquippedMeleeGO)
        mEquippedMeleeGO->SetEnabled(false);
    
    if (mEquippedGunGO && mUnEquippedGunGO)
        mUnEquippedGunGO->SetEnabled(false);

    if (mEquippedSpecialGO && mUnEquippedSpecialGO) 
    {
        mEquippedSpecialGO->SetEnabled(false);
        mUnEquippedSpecialGO->SetEnabled(false);
    }
        

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
        if (mGrenadeExplotionPreviewAreaGO) mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    }

    // ANIMATION
    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }
    //Hit Effect
    mGameObject->GetComponentsInChildren(ComponentType::MESHRENDERER, mMeshComponents);
    mMaterialIds.reserve(mMeshComponents.size());
    for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    {
        mMaterialIds.push_back(reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->GetResourceMaterial()->GetUID());
    }
    // Add Audio Listener
    if (mGameObject->GetComponent(ComponentType::AUDIOLISTENER) == nullptr)
    {
        mGameObject->CreateComponent(ComponentType::AUDIOLISTENER);
    }

    mUpperState->Enter();
    mLowerState->Enter();
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

    //Hit Effect
    //if (mHit)
    //{
    //    if (Delay(0.1f)) 
    //    {
    //        mHit = false;
    //
    //        for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    //        {
    //            reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->SetMaterial(mMaterialIds[i]);
    //            App->GetResource()->ReleaseResource(mMaterialIds[i]);
    //        }
    //    }
    //}
    mCollisionDirection = float3::zero;
}

bool PlayerController::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
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
        //LOG(("LOWER: " + std::to_string(type)).c_str());
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
                if (GetPlayerUpperState()->GetType() != StateType::ATTACK)
                {
                    if (GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
                        SetSpineAnimation("tIdleRanged", 0.3f);
                    else
                        SetSpineAnimation("tIdleMelee", 0.3f);
                }
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
        //LOG(("UPPER: " + std::to_string(type)).c_str());
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
    if (mLowerState->GetType() == StateType::DASH)
        return;

    GameManager* gameManager = GameManager::GetInstance();
    bool controller = gameManager->UsingController();

    if (controller)
    {
        float rightX = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
        float rightY = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);

        if (Abs(rightX) < 0.1f && Abs(rightY) < 0.1f) return;

        float3 position = mGameObject->GetWorldPosition();
        mAimPosition.x = position.x - rightX;
        mAimPosition.y = position.y;
        mAimPosition.z = position.z - rightY;
    }
    else
    {
        Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
        Plane plane(mGameObject->GetWorldPosition(), float3::unitY);

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

void PlayerController::SetAnimationSpeed(float speed)
{
    if (mAnimationComponent)
    {
        mAnimationComponent->SetAnimSpeed(speed);
    }
}

void PlayerController::MoveInDirection(float3 direction)
{
    float collisionDotProduct = direction.Dot(mCollisionDirection);
    if (collisionDotProduct < 0.0f)
    {
        mPlayerDirection = direction - mCollisionDirection.Mul(collisionDotProduct);
    }
    else
    {
        mPlayerDirection = direction;
    }

    float3 newPos = (mGameObject->GetLocalPosition() + mPlayerDirection * App->GetDt() * mPlayerSpeed);
    mGameObject->SetWorldPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(1.0f)));
}

void PlayerController::MoveToPosition(float3 position)
{
    mGameObject->SetWorldPosition(App->GetNavigation()->FindNearestPoint(position, float3(10.0f)));
}

void PlayerController::SwitchWeapon() 
{
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
    mAttackState->SetCooldown(mWeapon->GetAttackCooldown());
    if (mSpecialWeapon) mSpecialState->SetCooldown(mSpecialWeapon->GetAttackCooldown());
}

float3 PlayerController::GetPlayerPosition()
{
    return  mGameObject->GetWorldPosition(); 
}

void PlayerController::EquipMeleeWeapon(bool equip)
{
    if (mUnEquippedMeleeGO && mEquippedMeleeGO)
    {
        //true if you want to equip, false if unequip
        if (equip)
        {
            mUnEquippedMeleeGO->SetEnabled(false);
            mEquippedMeleeGO->SetEnabled(true);
        }
        else 
        {
            mEquippedMeleeGO->SetEnabled(false);
            mUnEquippedMeleeGO->SetEnabled(true);
        }
    }
}

void PlayerController::EquipRangedWeapons(bool equip)
{
    if (mUnEquippedGunGO && mEquippedGunGO)
    {
        //true if you want to equip, false if unequip
        if (equip)
        {
            mUnEquippedGunGO->SetEnabled(false);
            mEquippedGunGO->SetEnabled(true);
        }
        else
        {
            mUnEquippedGunGO->SetEnabled(true);
            mEquippedGunGO->SetEnabled(false);
        }
    }

    if (mSpecialWeapon!= nullptr && mUnEquippedSpecialGO && mEquippedSpecialGO)
    {
        //true if you want to equip, false if unequip
        if (equip)
        {
            mUnEquippedSpecialGO->SetEnabled(false);
            mEquippedSpecialGO->SetEnabled(true);
        }
        else
        {
            mUnEquippedSpecialGO->SetEnabled(true);
            mEquippedSpecialGO->SetEnabled(false);
        }
    }
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
    if (mGrenadeExplotionPreviewAreaGO)
    {
        mGrenadeExplotionPreviewAreaGO->SetEnabled(value);
        mGrenadeExplotionPreviewAreaGO->SetWorldScale(float3(mGrenade->GetGrenadeRadius(), mGrenade->GetGrenadeRadius(), 0.5f));
    }
}

void PlayerController::UpdateGrenadeVisuals()
{
    if (mGrenadeExplotionPreviewAreaGO)
    {
        float3 diff;
        if (GameManager::GetInstance()->UsingController())
        {
            mGrenadePosition = mGameObject->GetWorldPosition() + (mAimPosition - mGameObject->GetWorldPosition()) * mGrenadeRange;
        }
        else
        {
            diff = mAimPosition - mGameObject->GetWorldPosition();
            float distanceSquared = diff.LengthSq();
            float radiusSquared = mGrenadeRange * mGrenadeRange;

            if (distanceSquared <= radiusSquared)
            {
                mGrenadePosition = mAimPosition;
            }
            else
            {
                diff.Normalize();
                mGrenadePosition = mGameObject->GetWorldPosition() + diff * mGrenadeRange;
            }
        }

        mGrenadeExplotionPreviewAreaGO->SetWorldPosition(float3(mGrenadePosition.x, 0.1f, mGrenadePosition.z));
    }
}

void PlayerController::ThrowGrenade()
{
    // TODO wait for thow animation time
    if (mGrenade)
    {
        mGrenade->SetDestination(mGrenadePosition);
    }  
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
    const ModuleInput* input = App->GetInput();
    if (input->GetKey(Keys::Keys_G) == KeyState::KEY_DOWN)
    {
        mGodMode = !mGodMode;
    }
    if (input->GetKey(Keys::Keys_1) == KeyState::KEY_DOWN) 
    {
        RechargeBattery(EnergyType::BLUE);
    }
    else if (input->GetKey(Keys::Keys_2) == KeyState::KEY_DOWN) 
    {
        RechargeBattery(EnergyType::RED);
    }
    else if (input->GetKey(Keys::Keys_3) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
    }
    else if (input->GetKey(Keys::Keys_4) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level1Scene");
    }
    else if (input->GetKey(Keys::Keys_5) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level2Scene");
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
                mEquippedSpecialGO->SetEnabled(true);
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
                mEquippedSpecialGO->SetEnabled(true);
            }
            else
            {
                mSpecialWeapon = mHammer;
            }
            break;
        default:
            break;
    }

    mSpecialState->SetCooldown(mSpecialWeapon->GetAttackCooldown());
}

void PlayerController::UseEnergy(int energy)
{
    mCurrentEnergy -= energy;

    if (mCurrentEnergy <= 0)
    {
        mCurrentEnergy = 0;
        mEnergyType = EnergyType::NONE;
        mSpecialWeapon = nullptr;
        mEquippedSpecialGO->SetEnabled(false);
    }
        
    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType);
}

void PlayerController::TakeDamage(float damage)
{
    if (mLowerState->GetType() == StateType::DASH || mGodMode)
    {
        return;
    }

    if (mShield <= 0.0f)
    {
        GameManager::GetInstance()->GameOver();

        //CONTROLLER VIBRATION
        App->GetInput()->SetGameControllerRumble(20000, 30000, 100);
    }

    mShield = Clamp(mShield - damage, 0.0f, mMaxShield);

    //CONTROLLER VIBRATION
    App->GetInput()->SetGameControllerRumble(40000, 30000, 100);

    float healthRatio = mShield / mMaxShield;
    GameManager::GetInstance()->GetHud()->SetHealth(healthRatio);    


    ////Hit Effect
    //mHit = true;
    //for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    //{
    //    reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(mMaterialIds[i], Resource::Type::Material));
    //    reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->SetMaterial(999999999);
    //}
}

void PlayerController::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag() == "WinArea")
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level2Scene");
    }

    if (collisionData->collidedWith->GetTag() == "Door")
    {
        mCollisionDirection = collisionData->collisionNormal;
        LOG("HOLA")
    }
}