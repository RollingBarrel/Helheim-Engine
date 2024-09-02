#include "PlayerController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"

#include "GameObject.h"
#include "Physics.h"
#include "PlayerStats.h"

#include "AudioSourceComponent.h"
#include "BoxColliderComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"
#include "ImageComponent.h"

#include "AnimationComponent.h"

#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"

#include "Keys.h"
#include "Math/MathFunc.h"
#include "Geometry/Plane.h"

#include "GameManager.h"
#include "AudioManager.h"
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
#include "UltimateState.h"
#include "UltimateChargeState.h"

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

    //SEPARATOR("STATS");
    //MEMBER(MemberType::FLOAT, mMaxShield);
    //MEMBER(MemberType::FLOAT, mPlayerSpeed);

    SEPARATOR("SHIELD");
    MEMBER(MemberType::GAMEOBJECT, mHealParticles);
    MEMBER(MemberType::GAMEOBJECT, mShieldSpriteSheet);

    SEPARATOR("PICKUPS");
    MEMBER(MemberType::GAMEOBJECT, mRedBaterryParticles);
    MEMBER(MemberType::GAMEOBJECT, mBlueBaterryParticles);

    SEPARATOR("DASH");
    MEMBER(MemberType::FLOAT, mDashRange);
    MEMBER(MemberType::FLOAT, mDashCoolDown);
    MEMBER(MemberType::FLOAT, mDashDuration);
    MEMBER(MemberType::GAMEOBJECT, mDashVFX);

    SEPARATOR("RANGE");
    MEMBER(MemberType::GAMEOBJECT, mShootOrigin);

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
    MEMBER(MemberType::FLOAT, mGrenadeCursorSpeed);

    SEPARATOR("Ultimate");
    MEMBER(MemberType::GAMEOBJECT, mUltimateGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateChargeGO);
    MEMBER(MemberType::FLOAT, mUltimateCooldown);
    MEMBER(MemberType::FLOAT, mUltimateDuration);
    MEMBER(MemberType::FLOAT, mUltimateChargeDuration);
    MEMBER(MemberType::FLOAT, mUltimatePlayerSlow);
    MEMBER(MemberType::FLOAT, mUltimateDamageTick);
    MEMBER(MemberType::FLOAT, mUltimateDamageInterval);
    MEMBER(MemberType::FLOAT, mUltimateAimSpeed);

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
    //Player Stats
    mPlayerStats = App->GetScene()->GetPlayerStats();

    mMaxShield = mPlayerStats->GetMaxHealth();
    mShield = mMaxShield;

    mPlayerSpeed = mPlayerStats->GetSpeed();

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
    mUltimateState = new UltimateState(this, mUltimateCooldown, mUltimateDuration);
    mUltimateChargeState = new UltimateChargeState(this, 0.0f, mUltimateChargeDuration);

    mLowerStateType = StateType::IDLE;
    mUpperStateType = StateType::AIM;
    mUpperState = mAimState;
    mLowerState = mIdleState;


    // Weapons
    BoxColliderComponent* weaponCollider = nullptr;
    if (mMeleeCollider) 
    {
        weaponCollider = static_cast<BoxColliderComponent*>(mMeleeCollider->GetComponent(ComponentType::BOXCOLLIDER));
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
    //HEAL VFX
    if (mHealParticles) mHealParticles->SetEnabled(false);
    if (mShieldSpriteSheet) mShieldSpriteSheet->SetEnabled(false);

    //PICKUP VFX
    if (mRedBaterryParticles) mRedBaterryParticles->SetEnabled(false);
    if (mBlueBaterryParticles) mBlueBaterryParticles->SetEnabled(false);
    
    //ULTIMATE
    if (mUltimateGO)
        mUltimateGO->SetEnabled(false);

    if (mUltimateChargeGO)
        mUltimateChargeGO->SetEnabled(false);

    // COLLIDER
    mCollider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
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
        if (mGrenadeExplotionPreviewAreaGO) mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    }

    // ANIMATION
    mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }
    // Add Audio Listener
    if (mGameObject->GetComponent(ComponentType::AUDIOLISTENER) == nullptr)
    {
        mGameObject->CreateComponent(ComponentType::AUDIOLISTENER);
    }

    //Hit Effect
    mGameObject->GetComponentsInChildren(ComponentType::MESHRENDERER, mMeshComponents);

    for (Component* mesh : mMeshComponents)
    {
        const ResourceMaterial* material = static_cast<MeshRendererComponent*>(mesh)->GetResourceMaterial();
        mPlayerOgColor.push_back(material->GetBaseColorFactor());
    }

    mUpperState->Enter();
    mLowerState->Enter();

    //if (App->GetScene()->GetName().compare("Level1Scene") == 0)
    //{
    //    mGameObject->SetWorldPosition(float3(82.27f, 0.0f, -4.15f));
    //}
    //else if (App->GetScene()->GetName().compare("Level2Scene") == 0)
    //{
    //    mGameObject->SetWorldPosition(float3(163.02f, 65.72f, 12.90f));
    //}
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

    //Check HitEffect
    CheckHitEffect();

    // Buff, Debuff timers...
    CheckOtherTimers();

    CheckDebugOptions();
    mCollisionDirection = float3::zero;
}

void PlayerController::StateMachine()
{
    mLowerState->Update();
    mUpperState->Update();
}


void PlayerController::Paralyzed(float percentage, bool paralysis)
{
    if (paralysis)
    {
        if (!mIsParalyzed)
        {
            mIsParalyzed = true;
            mPlayerSpeed *= percentage;
            mParalyzedTimerScript = TimerScript();
            mParalysisSpeedReductionFactor = percentage;
        }
    }
    else
    {
        mIsParalyzed = false;
        mPlayerSpeed /= percentage;

        mParalysisSpeedReductionFactor = 1.0f;
    }
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
            case StateType::ULTIMATE:
                mUpperState = mUltimateState;
                break;
            case StateType::ULTIMATE_CHARGE:
                mUpperState = mUltimateChargeState;
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
        float3 cameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
        mAimPosition = position + ((cameraFront * -rightY) + (float3::unitY.Cross(cameraFront) * -rightX)).Normalized();
    }
    else
    {
        Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
        float3 planePoint = float3(mGameObject->GetWorldPosition().x ,mShootOrigin->GetWorldPosition().y, mGameObject->GetWorldPosition().z);
        Plane plane(planePoint, float3::unitY);

        float distance;
        if (plane.Intersects(ray, &distance))
        {
            mAimPosition = ray.GetPoint(distance);
            mAimPosition.y = mGameObject->GetWorldPosition().y;
        }
    }
    if (mUpperStateType != StateType::ULTIMATE)
        mGameObject->LookAt(mAimPosition);
    else
        UltimateInterpolateLookAt(mAimPosition);
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

    float3 newPos = (mGameObject->GetWorldPosition() + mPlayerDirection * App->GetDt() * mPlayerSpeed);
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

void PlayerController::SetMovementSpeed(float percentage) 
{
    mPlayerStats->SetSpeed(mPlayerStats->GetSpeed() * percentage);
    mPlayerSpeed = mPlayerStats->GetSpeed();
}

void PlayerController::SetWeaponDamage(float percentage)
{
    mPlayerStats->SetDamageModifier(mPlayerStats->GetDamageModifier() * percentage);
    mDamageModifier = mPlayerStats->GetDamageModifier();
}

void PlayerController::SetMaxShield(float percentage)
{
    mPlayerStats->SetMaxHealth(mPlayerStats->GetMaxHealth() * percentage);
    mMaxShield = mPlayerStats->GetMaxHealth();
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
            float rightX = - App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
            float rightY = - App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);

            if (!(std::abs(rightX) < 0.2f && std::abs(rightY) < 0.2f))
            {
                float3 position = mGameObject->GetWorldPosition();

                float3 cameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
                float3 cameraRight = float3::unitY.Cross(cameraFront).Normalized();

                float3 throwDirection = (cameraFront * rightY + cameraRight * rightX).Normalized();

                float3 movement = throwDirection * mGrenadeCursorSpeed * App->GetDt();
                mGrenadePosition += movement;
            }

            float3 diff = mGrenadePosition - mGameObject->GetWorldPosition();

            float distanceSquared = diff.LengthSq();
            float radiusSquared = mGrenadeRange * mGrenadeRange;
            if (distanceSquared > radiusSquared)
            {
                diff.Normalize();
                mGrenadePosition = mGameObject->GetWorldPosition() + diff * mGrenadeRange;
            }
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

        mGrenadeExplotionPreviewAreaGO->SetWorldPosition(float3(mGrenadePosition.x, mGameObject->GetWorldPosition().y, mGrenadePosition.z));
    }
}

void PlayerController::ThrowGrenade()
{
    // TODO wait for thow animation time
    if (mGrenade)
    {
        mGrenade->SetPositionDestination(mGameObject->GetWorldPosition(), mGrenadePosition);
    }  
}

void PlayerController::CheckOtherTimers()
{

    // Paralizys
    if (mIsParalyzed)
    {
        if (mParalyzedTimerScript.Delay(mParalyzedDuration))
        {
            Paralyzed(mParalysisSpeedReductionFactor, false);
        }
    }
}

bool PlayerController::CanReload() const
{
    if (mWeapon->GetCurrentAmmo() == mWeapon->GetMaxAmmo()) return false;

    return true;
}

void PlayerController::Reload() const
{
    static_cast<Pistol*>(mPistol)->Reload();
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
        if (mHealParticles) 
        {
            mHealParticles->SetEnabled(false);
            mHealParticles->SetEnabled(true);
        } 
        if (mShieldSpriteSheet) 
        {
            mShieldSpriteSheet->SetEnabled(true);
            reinterpret_cast<ImageComponent*>(mShieldSpriteSheet->GetComponent(ComponentType::IMAGE))->PlayAnimation();
        }
    }
}

void PlayerController::RechargeBattery(EnergyType batteryType)
{
    mCurrentEnergy = 100;
    mEnergyType = batteryType;

    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType);

    switch (mEnergyType)
    {
        case EnergyType::NONE:
            break;
        case EnergyType::BLUE:
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                if (mBlueBaterryParticles) 
                {
                    mBlueBaterryParticles->SetEnabled(false);
                    mBlueBaterryParticles->SetEnabled(true);
                }
                mSpecialWeapon = mMachinegun;
                mEquippedSpecialGO->SetEnabled(true);
            }
            else
            {
                if (mBlueBaterryParticles) 
                {
                    mBlueBaterryParticles->SetEnabled(false);
                    mBlueBaterryParticles->SetEnabled(true);
                }
                mSpecialWeapon = mKatana;
            }
            break;
        case EnergyType::RED:
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                if (mRedBaterryParticles) 
                {
                    mRedBaterryParticles->SetEnabled(false);
                    mRedBaterryParticles->SetEnabled(true);
                }
                mSpecialWeapon = mShootgun;
                mEquippedSpecialGO->SetEnabled(true);
            }
            else
            {
                if (mRedBaterryParticles) 
                {
                    mRedBaterryParticles->SetEnabled(false);
                    mRedBaterryParticles->SetEnabled(true);
                }
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

void PlayerController::AddUltimateResource()
{
    if (mUltimateResource != 100) 
        mUltimateResource += 10;
    else return;
}

void PlayerController::EnableUltimate(bool enable)
{
    if (mUltimateGO)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_ULTIMATE, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
        mUltimateGO->SetEnabled(enable);
    }
}

void PlayerController::EnableChargeUltimate(bool enable)
{
    if (mUltimateChargeGO)
    {
        mUltimateChargeGO->SetEnabled(enable);
    }
}


void PlayerController::UltimateInterpolateLookAt(const float3& target)
{
    float3 currentForward = mGameObject->GetFront().Normalized();

    float3 currZ = currentForward.Normalized();
    float3 currX = Cross(float3::unitY, currZ).Normalized();
    float3 currY = Cross(currZ, currX);

    float4x4 currentRotationMatrix = float4x4::identity;
    currentRotationMatrix[0][0] = currX.x; currentRotationMatrix[0][1] = currY.x; currentRotationMatrix[0][2] = currZ.x;
    currentRotationMatrix[1][0] = currX.y; currentRotationMatrix[1][1] = currY.y; currentRotationMatrix[1][2] = currZ.y;
    currentRotationMatrix[2][0] = currX.z; currentRotationMatrix[2][1] = currY.z; currentRotationMatrix[2][2] = currZ.z;

    Quat currentRotation = Quat(currentRotationMatrix);

    float3 targetForward = (target - mGameObject->GetWorldPosition()).Normalized();

    float3 targZ = targetForward.Normalized();
    float3 targX = Cross(float3::unitY, targZ).Normalized();
    float3 targY = Cross(targZ, targX);

    float4x4 targetRotationMatrix = float4x4::identity;
    targetRotationMatrix[0][0] = targX.x; targetRotationMatrix[0][1] = targY.x; targetRotationMatrix[0][2] = targZ.x;
    targetRotationMatrix[1][0] = targX.y; targetRotationMatrix[1][1] = targY.y; targetRotationMatrix[1][2] = targZ.y;
    targetRotationMatrix[2][0] = targX.z; targetRotationMatrix[2][1] = targY.z; targetRotationMatrix[2][2] = targZ.z;

    Quat targetRotation = Quat(targetRotationMatrix);

    Quat interpolatedRotation = Quat::Slerp(currentRotation, targetRotation, App->GetDt()*mUltimateAimSpeed);

    // Apply the interpolated rotation to the game object
    mGameObject->SetLocalRotation(interpolatedRotation);
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
    if (!mHit)
    {
        ActivateHitEffect();
    }


}
void PlayerController::ActivateHitEffect()
{
    for (Component* mesh: mMeshComponents)
    {
        MeshRendererComponent* meshComponent = static_cast<MeshRendererComponent*>(mesh);
        meshComponent->SetEnableBaseColorTexture(false);
        meshComponent->SetBaseColorFactor(float4(255.0f, 0.0f, 0.0f, 1.0f));
    }   
        mHit = true;    
}

void PlayerController::CheckHitEffect()
{
    if (mHit)
    {
        if (mHitEffectTimer.Delay(mHitEffectTime))
        {
            for (size_t i = 0; i < mMeshComponents.size(); i++)
            {
                MeshRendererComponent* meshComponent = static_cast<MeshRendererComponent*>(mMeshComponents[i]);
                meshComponent->SetEnableBaseColorTexture(true);
                meshComponent->SetBaseColorFactor(mPlayerOgColor[i]);
            }
            mHit = false;
        }
    }

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
        //LOG("HOLA")
    }
}