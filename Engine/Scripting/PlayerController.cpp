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
#include "Pistol.h"
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

    SEPARATOR("Grenade");
    MEMBER(MemberType::GAMEOBJECT, mGrenadeAimAreaGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeExplotionPreviewAreaGO);
    MEMBER(MemberType::FLOAT, mGrenadeRange);
    MEMBER(MemberType::FLOAT, mGrenadeCoolDown);

    SEPARATOR("DEBUG MODE");
    MEMBER(MemberType::BOOL, mGodMode);

    SEPARATOR("AUDIO");
    MEMBER(MemberType::GAMEOBJECT, mFootStepAudioHolder);
    MEMBER(MemberType::GAMEOBJECT, mGunfireAudioHolder);

    END_CREATE;
}

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{
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

    delete mMeleeWeapon;
    delete mRangeWeapon;
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
    mMeleeWeapon = new Bat();
    mRangeWeapon = new Pistol();
    mWeapon = mMeleeWeapon;

    // AUDIO
    if (mFootStepAudioHolder)
    {
        mFootStepAudio = (AudioSourceComponent*)mFootStepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mGunfireAudioHolder)
    {
        mGunfireAudio = (AudioSourceComponent*)mGunfireAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
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
    if (mGrenadeAimAreaGO && mGrenadeExplotionPreviewAreaGO)
    {
        ScriptComponent* script = (ScriptComponent*)mGrenadeExplotionPreviewAreaGO->GetComponent(ComponentType::SCRIPT);
        mGrenade = (Grenade*)script->GetScriptInstance();
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

        switch (type) {
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

        switch (type) {
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
    // TODO: Not aim on melee state?

    Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());
    Plane plane(mGameObject->GetPosition(), float3::unitY);

    float distance;
    if (plane.Intersects(ray, &distance))
    {
        mAimPosition = ray.GetPoint(distance);
        mGameObject->LookAt(mAimPosition);
    }
}

void PlayerController::SetAnimation(std::string trigger, float transitionTime)
{
    mAnimationComponent->SendTrigger(trigger, transitionTime);
}

void PlayerController::PlayOneShot(std::string name)
{
    if (strcmp(name.c_str(), "Step")) {
        mFootStepAudio->PlayOneShot();
    }
    if (strcmp(name.c_str(), "Shot")) {
        mGunfireAudio->PlayOneShot();
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
    if (mWeapon->GetType() == Weapon::WeaponType::RANGE) {
        mWeapon = mMeleeWeapon;
    }
    else {
        mWeapon = mRangeWeapon;
    }
}

float3 PlayerController::GetPlayerPosition()
{
    return  mGameObject->GetPosition(); 
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

    float3 diff = mAimPosition - mGameObject->GetPosition();
    float distanceSquared = diff.LengthSq();
    float radiusSquared = mGrenadeRange * mGrenadeRange;

    bool isWithinRange = distanceSquared <= radiusSquared;
    if (isWithinRange) {
        mGrenadePosition = mAimPosition;
    }
    else {
        diff.Normalize();
        mGrenadePosition = mGameObject->GetPosition() + diff * mGrenadeRange;
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