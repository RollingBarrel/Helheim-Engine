#include "PlayerController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
#include "Physics.h"

#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "AudioSourceComponent.h"
#include "BoxColliderComponent.h"
#include "CameraComponent.h"
#include "ScriptComponent.h"

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

void PlayerController::Start()
{
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

    //Animation
    mAnimationComponent = (AnimationComponent*)mGameObject->GetComponent(ComponentType::ANIMATION);
    if (mAnimationComponent)
    {
        mStateMachine = mAnimationComponent->GetStateMachine();

    }
    if (mStateMachine)
    {
        std::string clip = "Character";

        std::string defaultState = "default";
        std::string sIdle = "Idle";
        std::string sWalkForward = "Walk Forward";
        std::string sWalkBack = "Walk Back";
        std::string sStrafeLeft = "Strafe Left";
        std::string sStrafeRight = "Strafe Right";
        std::string sShooting = "Shooting";
        std::string sMeleeCombo = "MeleeCombo";

        std::string idleTrigger = "tIdle";
        std::string forwardTrigger = "tWalkForward";
        std::string backTrigger = "tWalkBack";
        std::string strafeLeftTrigger = "tStrafeLeft";
        std::string strafeRightTrigger = "tStrafeRight";
        std::string shootingTrigger = "tShooting";
        std::string meleeTrigger = "tMelee";

        mStateMachine->SetClipName(0, clip);

        //States
        mStateMachine->AddState(clip, sIdle);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sIdle), float(12.4f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sIdle), float(23.14f));

        mStateMachine->AddState(clip, sWalkForward);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkForward), float(5.78f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkForward), float(7.74f));

        mStateMachine->AddState(clip, sWalkBack);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkBack), float(7.8f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkBack), float(9.76f));

        mStateMachine->AddState(clip, sStrafeLeft);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeLeft), float(0.0f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeLeft), float(2.86f));

        mStateMachine->AddState(clip, sStrafeRight);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeRight), float(2.92f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeRight), float(5.72f));

        mStateMachine->AddState(clip, sShooting);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sShooting), float(9.8f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sShooting), float(12.36f));

        mStateMachine->AddState(clip, sMeleeCombo);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sMeleeCombo), float(23.9f));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sMeleeCombo), float(28.1f));


        //Transitions
        mStateMachine->AddTransition(defaultState, sIdle, idleTrigger);

        mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sIdle, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeRight, strafeRightTrigger);
        mStateMachine->AddTransition(sIdle, sShooting, shootingTrigger);
        mStateMachine->AddTransition(sIdle, sMeleeCombo, meleeTrigger);

        mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkForward, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeRight, strafeRightTrigger);
        mStateMachine->AddTransition(sWalkForward, sMeleeCombo, meleeTrigger);


        mStateMachine->AddTransition(sWalkBack, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkBack, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sWalkBack, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkBack, sStrafeRight, strafeRightTrigger);
        mStateMachine->AddTransition(sWalkBack, sMeleeCombo, meleeTrigger);


        mStateMachine->AddTransition(sStrafeLeft, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sStrafeRight, strafeRightTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sMeleeCombo, meleeTrigger);


        mStateMachine->AddTransition(sStrafeRight, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeRight, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sStrafeRight, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeRight, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sStrafeRight, sMeleeCombo, meleeTrigger);


        mStateMachine->AddTransition(sShooting, sIdle, idleTrigger);
        mStateMachine->AddTransition(sShooting, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sShooting, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sShooting, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sShooting, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sMeleeCombo, sIdle, idleTrigger);

        mAnimationComponent->OnStart();
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

    Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetLocalMousePosition());
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

#pragma region Shield

void PlayerController::RechargeShield(float shield)
{
    if (mShield < mMaxShield) 
    {
        mShield = Clamp(mShield + shield, 0.0f, mMaxShield);
        UpdateShield();
    }
}

void PlayerController::TakeDamage(float damage)
{
    if (mLowerState->GetType() == StateType::DASH || mGodMode)
    {
        return;
    }

    mShield = Clamp(mShield - damage, 0.0f, mMaxShield);
    UpdateShield();

    if (mShield < 0.0f)
    {
        GameManager::GetInstance()->GameOver();
    }
}

void PlayerController::UpdateShield()
{
    float healthRatio = mShield / mMaxShield;
    GameManager::GetInstance()->GetHud()->SetHealth(healthRatio);
}

#pragma endregion

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


