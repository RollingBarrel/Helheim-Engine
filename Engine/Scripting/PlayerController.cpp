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
#include "Geometry/Line.h"
#include "Geometry/LineSegment.h"

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
#include <LineComponent.h>
#include "UltimateAttack.h"
#include "PlayerCamera.h"

CREATE(PlayerController)
{
    CLASS(owner);

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
    MEMBER(MemberType::GAMEOBJECT, mCharacterMesh);

    SEPARATOR("RANGE");
    MEMBER(MemberType::GAMEOBJECT, mShootOrigin);
    MEMBER(MemberType::FLOAT, mLaserLenght);
    MEMBER(MemberType::FLOAT, mControllerAimSpeed);

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
    MEMBER(MemberType::GAMEOBJECT, mGrenadeThrowOrigin);
    MEMBER(MemberType::FLOAT, mGrenadeRange);
    MEMBER(MemberType::FLOAT, mGrenadeCoolDown);
    MEMBER(MemberType::FLOAT, mGrenadeCursorSpeed);

    SEPARATOR("Ultimate");
    MEMBER(MemberType::GAMEOBJECT, mUltimateGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateChargeGO);
    MEMBER(MemberType::GAMEOBJECT, mUltiOuterChargeGO);
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
    mPlayerStats->SetInitLevelStats();

    mMaxShield = mPlayerStats->GetMaxHealth();
    mShield = mMaxShield;
    mGodMode = mPlayerStats->GetGodMode();
    mPlayerSpeed = mPlayerStats->GetSpeed();
    mDamageModifier = mPlayerStats->GetDamageModifier();
    mUltimateResource = mPlayerStats->GetUltimateResource();

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

    if (mUltiOuterChargeGO)
        mUltiOuterChargeGO->SetEnabled(false);

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
    mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponentInChildren(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
        mAnimationComponent->SendTrigger("tIdle", 0.1f);
        mAnimationComponent->SendSpineTrigger("tAim", 0.1f);

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

    //LASER
    GameObject* laserFinalPoint = mShootOrigin->GetChildren()[0];
    if (laserFinalPoint) laserFinalPoint->SetWorldPosition(mShootOrigin->GetWorldPosition() - mGameObject->GetRight() * mLaserLenght);
    
}

void PlayerController::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    if (GameManager::GetInstance()->IsPlayingCinematic()) return;

    if (mIsInElevator) return;
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

bool PlayerController::IsPlayerDashing() const
{ 
    return mLowerState->GetType() == StateType::DASH;
}

void PlayerController::SetPlayerEmisive(const float3& emisiveColor)
{
    for (Component* mesh : mMeshComponents)
    {
        MeshRendererComponent* meshComponent = static_cast<MeshRendererComponent*>(mesh);
        meshComponent->SetEmissiveColor(emisiveColor);
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
                    /*
                    if (GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
                        SetSpineAnimation("tIdleRanged", 0.3f);
                    else
                        SetSpineAnimation("tIdleMelee", 0.3f);
                    */
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
    if (mLowerState->GetType() == StateType::DASH) return;

     
    bool isUsingController = GameManager::GetInstance()->UsingController();
    if (isUsingController)
    {
        float rightX = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
        float rightY = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);

        if (Abs(rightX) < 0.1f && Abs(rightY) < 0.1f) return;

        float3 position = mShootOrigin->GetWorldPosition();
        position.y = mGameObject->GetWorldPosition().y;
        float3 cameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
        mAimPosition = position + ((cameraFront * -rightY) + (float3::unitY.Cross(cameraFront) * -rightX)).Normalized();

        Line aimLine = Line(position, (mAimPosition - position).Normalized());
        LineSegment lineSegment = LineSegment(aimLine, 10.0f);
        float minimunDistance = 1.0f;

        GameObject* closestEnemy = nullptr;
        float closestDistance = FLT_MAX;
        const std::vector<GameObject*>& allEnemies = App->GetScene()->FindGameObjectsWithTag("Enemy");
        for (GameObject* enemy : allEnemies)
        {
            if (enemy->IsEnabled() && enemy->GetName().compare("OB_explosive_Barrell") != 0)
            {
                float distance = enemy->GetWorldPosition().Distance(lineSegment);

                if (distance < minimunDistance && distance < closestDistance)
                {
                    if ((mAimPosition - mGameObject->GetWorldPosition()).AngleBetween(enemy->GetWorldPosition() - mGameObject->GetWorldPosition()) < DegToRad(35.0f))
                    {
                        closestDistance = distance;
                        closestEnemy = enemy;
                    }
                }
            } 
        }

        if (closestEnemy)
        {
            mAimPosition = closestEnemy->GetWorldPosition();
            mAimPosition.y = position.y;
        }
    }
    else
    {
        Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
        float3 planePoint = float3(mGameObject->GetWorldPosition().x, mShootOrigin->GetWorldPosition().y, mGameObject->GetWorldPosition().z);
        Plane plane(planePoint, float3::unitY);

        float distance;
        if (plane.Intersects(ray, &distance))
        {
            float3 rayPoint = ray.GetPoint(distance);
            mAimPosition = rayPoint;
            //mDebugCube->SetWorldPosition(mAimPosition);
            mAimPosition.y = mGameObject->GetWorldPosition().y; 
        }    
    }

    if (mUpperStateType != StateType::ULTIMATE)
    {
        if (!isUsingController)
        {
            mGameObject->LookAt(mAimPosition);
        }
        else
        {
            InterpolateLookAt(mAimPosition, mControllerAimSpeed);
        }
        HandleLaser();
    }   
    else
    {
        InterpolateLookAt(mAimPosition, mUltimateAimSpeed);
    }      
}

void PlayerController::HandleLaser()
{
    GameObject* laserFinalPoint = mShootOrigin->GetChildren()[0];
    if (laserFinalPoint)
    {
        Hit hit;
        Ray ray;
        ray.pos = mShootOrigin->GetWorldPosition();
        ray.dir = mGameObject->GetFront();
        std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "Bridge", "DoorArea", "Collectible" };
        Physics::Raycast(hit, ray, mLaserLenght, &ignoreTags);

        float rayLenght = (hit.IsValid()) ? hit.mDistance : mLaserLenght;
        laserFinalPoint->SetWorldPosition(mShootOrigin->GetWorldPosition() + mGameObject->GetFront() * rayLenght);
    }  
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

void PlayerController::SetLowerAnimationSpeed(float speed)
{
    if (mAnimationComponent)
    {
        mAnimationComponent->SetLowerAnimSpeed(speed);
    }
}

void PlayerController::SetAnimationTime(float time)
{
    if (mAnimationComponent)
    {
        mAnimationComponent->SetControllerTime(time);
    }
}

void PlayerController::SetIsAnimationPlaying(bool state)
{
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(state);
    }
}

void PlayerController::RestartAnimationState()
{
    if (mAnimationComponent)
    {
        mAnimationComponent->RestartStateAnimation();
    }
}

void PlayerController::DashLookAtFront()
{
    float3 dir = GetPlayerDirection();
    float3 pos = mGameObject->GetWorldPosition();
    mGameObject->LookAt(float3(dir.x + pos.x, pos.y, dir.z + pos.z));
}

std::string PlayerController::GetLowerAnimState() const
{
    if (mAnimationComponent)
    {
        return mAnimationComponent->GetCurrentStateName();
    }
    return std::string();
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

void PlayerController::EnableLaser(bool enable)
{
    LineComponent* lineComponent = static_cast<LineComponent*>(mShootOrigin->GetComponent(ComponentType::LINE));
    if (lineComponent) lineComponent->SetEnable(enable);
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

void PlayerController::SetMovementSpeedStat(float percentage)
{
    mPlayerSpeed = mPlayerStats->GetSpeed() * percentage;
    mPlayerStats->SetSpeed(mPlayerSpeed);
}

void PlayerController::SetSpeed(float speed)
{
    mPlayerSpeed = speed;
}

void PlayerController::SetWeaponDamage(float percentage)
{
    if (mDamageModifier < 5000.0f) {
        mDamageModifier = mPlayerStats->GetDamageModifier() * percentage;
        mPlayerStats->SetDamageModifier(mDamageModifier);
    }
}

void PlayerController::SetMaxShield(float percentage)
{
    mMaxShield = mPlayerStats->GetMaxHealth() * percentage;
    mPlayerStats->SetMaxHealth(mMaxShield);
    GameManager::GetInstance()->GetHud()->SetMaxHealth(mMaxShield);
}

void PlayerController::EnableGrenadeAim(bool value)
{
    EnableLaser(!value);
    if (mGrenadeExplotionPreviewAreaGO)
    {
        mGrenadeExplotionPreviewAreaGO->SetEnabled(value);
        if (value)
        {
            mGrenadeGO->SetWorldScale(float3(mGrenade->GetGrenadeRadius()));
            mGrenadeExplotionPreviewAreaGO->SetWorldPosition(mGameObject->GetWorldPosition());
        }  
    }
}

void PlayerController::GrenadeAim()
{
    if (mGrenadeExplotionPreviewAreaGO)
    {
        if (GameManager::GetInstance()->UsingController())
        {
            mGrenadeExplotionPreviewAreaGO->SetEnabled(true);
            float rightX = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX);
            float rightY = App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY);

            float2 rightStickVector = float2(rightX, rightY);
            float lenght = rightStickVector.Length();
            lenght = Min(1.0f, lenght);

            float3 cameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
            float3 cameraRight = float3::unitY.Cross(cameraFront).Normalized();
            float3 aimDirection = ((cameraFront * -rightStickVector.y) + (cameraRight * -rightStickVector.x)).Normalized();

            std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "DoorArea", "LineFinal" "Collectible", "Rat", "Enemy", "WinArea" };
            Hit hit;
            Ray playerRay;

            playerRay.dir = aimDirection;
            playerRay.pos = mGameObject->GetWorldPosition() + float3::unitY;
            Physics::Raycast(hit, playerRay, 100.0f, &ignoreTags);

            if (hit.IsValid() && hit.mDistance < mGrenadeRange)
            {
                if (!mGrenadeAimTimer.DelayWithoutReset(0.15f))
                {
                    mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
                }
                mGrenadePosition = App->GetNavigation()->FindNearestPoint(hit.mHitPoint, float3(10.0f, 1.0f, 10.0f));
            }
            else 
            {
                if (!mGrenadeAimTimer.DelayWithoutReset(0.15f))
                {
                    lenght = 1.0f;
                    aimDirection = mGameObject->GetFront();
                    mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
                }
                mGrenadePosition = mGameObject->GetWorldPosition() + aimDirection * (mGrenadeRange * lenght);
                mGrenadePosition = App->GetNavigation()->FindNearestPoint(mGrenadePosition, float3(10.0f, 1.0f, 10.0f));
            }  
        }
        else
        {
            Ray mouseRay = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
            float3 planePoint = mGameObject->GetWorldPosition();
            Plane plane(planePoint, float3::unitY);
            float grenadePlaneDistance;
            if (plane.Intersects(mouseRay, &grenadePlaneDistance))
            {
                float3 rayPoint = mouseRay.GetPoint(grenadePlaneDistance);
                float3 initialPosition = mGameObject->GetWorldPosition();
                float3 aimDirection = rayPoint - mGameObject->GetWorldPosition();
                float distanceSquared = aimDirection.LengthSq();
                float radiusSquared = mGrenadeRange * mGrenadeRange;
                aimDirection.Normalize();

                std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "DoorArea", "LineFinal" "Collectible", "Rat", "Enemy", "WinArea" };
                Hit hit;
                Ray playerRay;

                playerRay.dir = aimDirection;
                playerRay.pos = mGameObject->GetWorldPosition();
                Physics::Raycast(hit, playerRay, 100.0f, &ignoreTags);

                if (hit.IsValid() && hit.mDistance < mGrenadeRange)
                {
                    mGrenadePosition = App->GetNavigation()->FindNearestPoint(hit.mHitPoint, float3(10.0f, 1.0f, 10.0f));
                }
                else
                {
                    mGrenadePosition = (distanceSquared < radiusSquared) ? rayPoint : initialPosition + aimDirection.Normalized() * mGrenadeRange;
                    mGrenadePosition = App->GetNavigation()->FindNearestPoint(mGrenadePosition, float3(10.0f, 1.0f, 10.0f));
                }
            }
        }
        mGrenadeExplotionPreviewAreaGO->SetWorldPosition(mGrenadePosition);
    } 
}

void PlayerController::ThrowGrenade()
{
    // TODO wait for thow animation time
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_THROW, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

    if (mGrenade) mGrenade->ThrowGrenade(mGrenadeThrowOrigin->GetWorldPosition(), mGrenadePosition + float3(0.0f, 0.8f, 0.0f));
    mGrenadeAimTimer.Reset();
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
        App->GetScene()->GetPlayerStats()->SetGodMode(mGodMode);
        GameManager::GetInstance()->GetHud()->SetGodmode(mGodMode);
    }
    if (input->GetKey(Keys::Keys_K) == KeyState::KEY_DOWN)
    {
        if (mDamageModifier != 99999.0f)
        {
            mDamageModifier = 99999.0f;
            App->GetScene()->GetPlayerStats()->SetDamageModifier(mDamageModifier);
            GameManager::GetInstance()->GetHud()->SetInstaKill(true);
        }
        else
        {
            App->GetScene()->GetPlayerStats()->SetDamageModifier(mDamageModifier);
            mDamageModifier = 1.0f;
            GameManager::GetInstance()->GetHud()->SetInstaKill(false);
        }
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
        AddUltimateResource();
        AddUltimateResource();
        AddUltimateResource();
        AddUltimateResource();
        AddUltimateResource();
        mPlayerStats->SetUltimateResource(mUltimateResource);
    }
    else if (input->GetKey(Keys::Keys_F7) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
        App->GetScene()->GetPlayerStats()->ResetStats();
    }
    else if (input->GetKey(Keys::Keys_F8) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level1Scene");
        App->GetScene()->GetPlayerStats()->ResetStats();
    }
    else if (input->GetKey(Keys::Keys_F9) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level2Scene");
        App->GetScene()->GetPlayerStats()->TryAgainStats();
    }    
    else if (input->GetKey(Keys::Keys_F10) == KeyState::KEY_DOWN)
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level3Scene");
        App->GetScene()->GetPlayerStats()->TryAgainStats();
    }
    
}

void PlayerController::RechargeShield(float shield)
{
    if (mShield < mMaxShield) 
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_HEAL, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

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
        }
    }
}

void PlayerController::RechargeBattery(EnergyType batteryType)
{
    if(mEnergyType!= batteryType) mCurrentEnergy = 0;
    mCurrentEnergy = Clamp(mCurrentEnergy+30,0,90);
    mEnergyType = batteryType;

    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType, true);

    float3 emisiveColor = float3::one;

    switch (mEnergyType)
    {
        case EnergyType::NONE:
            break;
        case EnergyType::BLUE:
            emisiveColor = float3(0.0f, 0.73f, 1.0f);
            if (mBlueBaterryParticles)
            {
                mBlueBaterryParticles->SetEnabled(false);
                mBlueBaterryParticles->SetEnabled(true);
            }
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                mSpecialWeapon = mMachinegun;
                if(mEquippedSpecialGO) mEquippedSpecialGO->SetEnabled(true);
            }
            else
            {
                mSpecialWeapon = mKatana;
            }
            break;
        case EnergyType::RED:
            emisiveColor = float3(1.0f, 0.15f, 0.0f);
            if (mRedBaterryParticles)
            {
                mRedBaterryParticles->SetEnabled(false);
                mRedBaterryParticles->SetEnabled(true);
            }
            if (mWeapon->GetType() == Weapon::WeaponType::RANGE)
            {
                mSpecialWeapon = mShootgun;
                if(mEquippedSpecialGO) mEquippedSpecialGO->SetEnabled(true);
            }
            else
            {
                mSpecialWeapon = mHammer;
            }
            break;
    }

    SetPlayerEmisive(emisiveColor);




    mSpecialState->SetCooldown(mSpecialWeapon->GetAttackCooldown());
}

void PlayerController::UseEnergy(int energy)
{
    mCurrentEnergy -= energy;

    if (mCurrentEnergy <= 0)
    {
        mCurrentEnergy = 0;
        mEnergyType = EnergyType::NONE;
        SetPlayerEmisive(float3::one);
        mSpecialWeapon = nullptr;

        if(mEquippedSpecialGO)
            mEquippedSpecialGO->SetEnabled(false);
    }
        
    GameManager::GetInstance()->GetHud()->SetEnergy(mCurrentEnergy, mEnergyType, false);
}

void PlayerController::ResetEnergy()
{
    mCurrentEnergy = 0;
    mEnergyType = EnergyType::NONE;
}

void PlayerController::AddUltimateResource()
{
    if ( mUltimateResource != 100)
    {
        mUltimateResource += 20;
        GameManager::GetInstance()->GetHud()->SetUltimateCooldown(mUltimateResource);
        mPlayerStats->SetUltimateResource(mUltimateResource);
    }
    else return;
}

void PlayerController::UseUltimateResource()
{
    mUltimateResource = 0;
    mPlayerStats->SetUltimateResource(mUltimateResource);
}

void PlayerController::EnableUltimate(bool enable)
{
    UltimateAttack* ultimateScript = (UltimateAttack*)((ScriptComponent*)mUltimateGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
    ultimateScript->ResetTimer();
    if (mUltimateGO)
    {
        mInUlt = enable;
        if(enable) GameManager::GetInstance()->GetPlayerCamera()->ActivateShake(4.0f, 0.1f);
        else GameManager::GetInstance()->GetAudio()->Pause(SFX::PLAYER_ULTIMATE,mUltSound,true);
        mUltimateGO->SetEnabled(enable);
    }
}

void PlayerController::EnableChargeUltimate(bool enable)
{
    if (mUltimateChargeGO)
    {
        mInUlt = enable;
        if(enable) mUltSound = GameManager::GetInstance()->GetAudio()->Play(SFX::PLAYER_ULTIMATE); 
        mUltimateChargeGO->SetEnabled(enable);
        mUltiOuterChargeGO->SetEnabled(enable);
    }
}

void PlayerController::InterpolateLookAt(const float3& target, float speed)
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

    Quat interpolatedRotation = Quat::Slerp(currentRotation, targetRotation, App->GetDt()*speed);

    // Apply the interpolated rotation to the game object
    mGameObject->SetLocalRotation(interpolatedRotation);
}

void PlayerController::TakeDamage(float damage)
{
    //GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_HIT, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
    if (IsPlayerDashing()|| mGodMode || mInUlt)
    {
        return;
    }

    if (mShield <= 0.0f)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_DEATH1, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_DEATH2, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

        GameManager::GetInstance()->GameOver();

        //CONTROLLER VIBRATION
        App->GetInput()->SetGameControllerRumble(20000, 30000, 100);
        return;
    }

    mShield = Clamp(mShield - damage, 0.0f, mMaxShield);

    // Last opportunity
    if (mShield == 0.0f)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_BROKEN, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_DANGER, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());
    }

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

void PlayerController::AddKill()
{
    mKillCount++;
    if (mUpperStateType != StateType::ULTIMATE && mUltimateUnlocked) AddUltimateResource(); 
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
        if(App->GetScene()->GetName() == "Level1Scene")
            GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level2Scene");  
        else if(App->GetScene()->GetName() == "Level2Scene")
            GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level3Scene");

        return;
    }

    if (collisionData->collidedWith->GetTag() == "Door" || collisionData->collidedWith->GetTag() == "Bridge" || collisionData->collidedWith->GetTag() == "Enemy")
    {
        mCollisionDirection = collisionData->collisionNormal;
        //LOG("HOLA")
    }
}