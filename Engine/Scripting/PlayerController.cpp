#include "PlayerController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleDetourNavigation.h"
#include "Physics.h"
#include "ModuleScene.h"

#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "AudioSourceComponent.h"
#include "SliderComponent.h"
#include "BoxColliderComponent.h"
#include "CameraComponent.h"

#include "Keys.h"
#include "Math/MathFunc.h"
#include "Geometry/Plane.h"
#include <functional>

#include "MathConstants.h"
#include "EnemyExplosive.h"
#include "EnemyRobot.h"
#include "ObjectPool.h"
#include "Quadtree.h"
#include "GameManager.h"
#include "HudController.h"
#include "RangeWeapon.h"
#include "Grenade.h"

CREATE(PlayerController)
{
    CLASS(owner);

    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxShield);
    MEMBER(MemberType::FLOAT, mMaxSanity);
    MEMBER(MemberType::FLOAT, mPlayerSpeed);

    SEPARATOR("DASH");
    MEMBER(MemberType::FLOAT, mDashRange);
    MEMBER(MemberType::FLOAT, mDashCoolDown);
    MEMBER(MemberType::FLOAT, mDashDuration);

    SEPARATOR("MELEE ATTACK");
    MEMBER(MemberType::FLOAT, mMeleeBaseDamage);
    MEMBER(MemberType::FLOAT, mMeleeBaseRange);
    MEMBER(MemberType::FLOAT, mMeleeSpecialDamage);
    MEMBER(MemberType::FLOAT, mMeleeSpecialRange);

    SEPARATOR("RANGE ATTACK");
    MEMBER(MemberType::FLOAT, mRangeBaseDamage);
    MEMBER(MemberType::INT, mAmmoCapacity);
    MEMBER(MemberType::GAMEOBJECT, mRangeWeaponGameObject);
    

    SEPARATOR("Grenade");
    MEMBER(MemberType::GAMEOBJECT, mGrenadeAimAreaGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeExplotionPreviewAreaGO);
    MEMBER(MemberType::FLOAT, mGrenadThrowDistance);
    MEMBER(MemberType::FLOAT, mGrenadeCoolDown);


    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);

    SEPARATOR("DEBUG MODE");
    MEMBER(MemberType::BOOL, mGodMode);

    SEPARATOR("WIN AREA");
    MEMBER(MemberType::GAMEOBJECT, mWinArea);
    SEPARATOR("AUDIO");
    MEMBER(MemberType::GAMEOBJECT, mFootStepAudioHolder);
    MEMBER(MemberType::GAMEOBJECT, mGunfireAudioHolder);

    SEPARATOR("BULLET POOL");
    MEMBER(MemberType::GAMEOBJECT, mBulletPoolHolder);

    END_CREATE;
}

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{
}

void PlayerController::Start()
{
    // TODO remove this line after testing
    GameManager::GetInstance()->GetPlayer();

    if (mGameManagerGO)
    {
        ScriptComponent* script = (ScriptComponent*)mGameManagerGO->GetComponent(ComponentType::SCRIPT);
        mGameManager = (GameManager*)script->GetScriptInstance();
    }

    mBullets = mAmmoCapacity;
    mShield = mMaxShield;
    mSanity = mMaxSanity;

    if (mHudControllerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mHudControllerGO->GetComponent(ComponentType::SCRIPT));
        mHudController = static_cast<HudController*>(script->GetScriptInstance());
    }
    
    //Weapons
    if (mRangeWeaponGameObject)
    {
        mRangeWeapon = reinterpret_cast<RangeWeapon*>(reinterpret_cast<ScriptComponent*>(mRangeWeaponGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    }

    if (mFootStepAudioHolder)
    {
        mFootStepAudio = (AudioSourceComponent*)mFootStepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mGunfireAudioHolder)
    {
        mGunfireAudio = (AudioSourceComponent*)mGunfireAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mBulletPoolHolder)
    {
        mBulletPool = (ObjectPool*)((ScriptComponent*)mBulletPoolHolder->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
    }

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&PlayerController::OnCollisionEnter, this, std::placeholders::_1)));
    }

    // CAMERA
    mCamera = App->GetCamera()->GetCurrentCamera()->GetOwner();
    
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

        std::string idleTrigger = "tIdle";
        std::string forwardTrigger = "tWalkForward";
        std::string backTrigger = "tWalkBack";
        std::string strafeLeftTrigger = "tStrafeLeft";
        std::string strafeRightTrigger = "tStrafeRight";
        std::string shootingTrigger = "tShooting";

        mStateMachine->SetClipName(0, clip);

        //States
        mStateMachine->AddState(clip, sIdle);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sIdle), float(6.2));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sIdle), float(11.57));

        mStateMachine->AddState(clip, sWalkForward);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkForward), float(2.89));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkForward), float(3.87));

        mStateMachine->AddState(clip, sWalkBack);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkBack), float(3.90));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkBack), float(4.88));

        mStateMachine->AddState(clip, sStrafeLeft);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeLeft), float(0.0));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeLeft), float(1.43));

        mStateMachine->AddState(clip, sStrafeRight);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sStrafeRight), float(1.46));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sStrafeRight), float(2.85));

        mStateMachine->AddState(clip, sShooting);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sShooting), float(4.9));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sShooting), float(6.18));

        //Transitions
        mStateMachine->AddTransition(defaultState, sIdle, idleTrigger);


        mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sIdle, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeRight, strafeRightTrigger);
        mStateMachine->AddTransition(sIdle, sShooting, shootingTrigger);

        mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkForward, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sWalkBack, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkBack, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sWalkBack, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkBack, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sStrafeLeft, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sStrafeRight, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeRight, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sStrafeRight, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeRight, sStrafeLeft, strafeLeftTrigger);

        mStateMachine->AddTransition(sShooting, sIdle, idleTrigger);
        mStateMachine->AddTransition(sShooting, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sShooting, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sShooting, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sShooting, sStrafeRight, strafeRightTrigger);

        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);
    }

    //END ANIMATION
}

void PlayerController::Update()
{
    CheckDebugOptions();
    UpdateGrenadeCooldown();
    UpdateBattleSituation();

    if (mIsDashCoolDownActive)
    {
        mDashCoolDownTimer += App->GetDt();
        if (mDashCoolDownTimer >= mDashCoolDown)
        {
            mDashCoolDownTimer = 0.0f;
            mIsDashCoolDownActive = false;
            mCurrentState = PlayerState::IDLE;
        }
    }

    if (mIsMeleeBaseComboActive)
    {
        mMeleeBaseComboTimer += App->GetDt();
        if (mMeleeBaseComboTimer >= mMeleeBaseMaxComboInterval)
        {
            mMeleeBaseComboStep = 1;
            mIsMeleeBaseComboActive = false;
            mMeleeBaseComboTimer = 0.0f;
            mCurrentState = PlayerState::IDLE;
        }
    }

    if (mIsMeleeSpecialCoolDownActive)
    {
        mMeleeSpecialCoolDownTimer += App->GetDt();
        if (mMeleeSpecialCoolDownTimer >= mMeleeSpecialCoolDown)
        {
            mMeleeSpecialCoolDownTimer = 0.0f;
            mIsMeleeSpecialCoolDownActive = false;
            mCurrentState = PlayerState::IDLE;
        }
    }

    switch (mCurrentState)
    {
    case PlayerState::IDLE:
        if (!mVictory && !mGameOver)
        {   
            if (mAnimationComponent)
            {
                mAnimationComponent->SendTrigger("tIdle", 0.1f);
            }
            Idle();


        }
        break;

    case PlayerState::DASH:
        Dash();
        break;

    case PlayerState::MOVE:
        Moving();
       
        break;

    case PlayerState::ATTACK:
        Attack();
        break;

    case PlayerState::MOVE_ATTACK:
        Moving();
        Attack();
        break;

    case PlayerState::DEATH:
        Death();
        break;
    }

    HandleRotation();

    if (mWinArea && mGameObject->GetPosition().Distance(mWinArea->GetPosition()) < 2.0f)
    {
        mHudController->SetScreen(SCREEN::WIN, true);
    }

    Loading();
}


void PlayerController::Idle()
{
    
    if (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        mWeapon = (mWeapon == WeaponType::RANGE) ? WeaponType::MELEE : WeaponType::RANGE;
    }

    else if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_REPEAT && !mThrowAwayGrenade)
    {
        mCurrentState = PlayerState::ATTACK;
        mAimingGrenade = true;
    }
    
    else if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN && !mIsDashCoolDownActive)
    {
	    mCurrentState = PlayerState::DASH;
    }
    else 
    {
        if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
        {
            mCurrentState = PlayerState::MOVE;
        }
        else
        {
            mCurrentState = PlayerState::IDLE;
        }
    
        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
        {
            mCurrentState = (mCurrentState == PlayerState::MOVE) ? PlayerState::MOVE_ATTACK : PlayerState::ATTACK;
            mLeftMouseButtonPressed = true;
        }
        else if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
            mCurrentState = (mCurrentState == PlayerState::MOVE) ? PlayerState::MOVE_ATTACK : PlayerState::ATTACK;
            mLeftMouseButtonPressed = false;
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        if (mGrenadeAimAreaGO) 
        {
            mGrenadeAimAreaGO->SetEnabled(false);
        }
    }
}

void PlayerController::Moving()
{
    mMoveDirection = float3::zero;
    float3 front = mCamera->GetRight().Cross(float3::unitY).Normalized(); 

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += front;

    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= front;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += float3::unitY.Cross(front);

    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= float3::unitY.Cross(front);

    }

    if (!mMoveDirection.Equals(float3::zero))
    {
        mMoveDirection.Normalize(); 
        SetMovingDirection(mMoveDirection);
        float2 mousePosition(App->GetInput()->GetGlobalMousePosition());
        ClosestMouseDirection(mousePosition);
        switch (mLookingAt)
        {
        case MouseDirection::UP:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            default:
                break;
            }

            break;
        case MouseDirection::DOWN:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT: 
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            default:
                break;
            }

            break;
        case MouseDirection::LEFT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            default:
                break;
            }
            break;
        case MouseDirection::RIGHT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            default:
                break;
            }
            break;

        case MouseDirection::UP_RIGHT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:

                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);

                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            default:
                break;
            }
            break;
        case MouseDirection::UP_LEFT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:

                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);

                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            default:
                break;
            }
            break;
        case MouseDirection::DOWN_RIGHT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            default:
                break;
            }
            break;
        case MouseDirection::DOWN_LEFT:
            switch (mMovingTo)
            {
            case MoveDirection::UP:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::RIGHT:
                mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
                break;
            case MoveDirection::LEFT:
                mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
                break;
            case MoveDirection::UP_RIGHT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break; 
            case MoveDirection::UP_LEFT:
                mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
                break;
            case MoveDirection::DOWN_RIGHT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            case MoveDirection::DOWN_LEFT:
                mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        Move(mMoveDirection);
    }
    else {
        mAnimationComponent->SendTrigger("tIdle", 0.1f);
    }

    // Hardcoded play-step-sound solution: reproduce every second 
    // TODO play sound according the animation
    
    if (!mReadyToStep)
    {
        mStepTimePassed += App->GetDt();
        if (mStepTimePassed >= mStepCoolDown)
        {
            mStepTimePassed = 0;
            mStepTimePassed = false;
            mReadyToStep = true;
        }
    }
    else
    {
        mFootStepAudio->PlayOneShot();
        mReadyToStep = false;
    }
  
    Idle();
}

void PlayerController::Move(float3 direction) 
{
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetDt() * mPlayerSpeed);

    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
}

void PlayerController::HandleRotation()
{
    if ((mCurrentState == PlayerState::ATTACK && mIsMeleeSpecialCoolDownActive) ||
        (mWeapon == WeaponType::MELEE && mMeleeBaseComboStep == 3))
    {
        return;
    }

    std::map<float, Hit> hits;
    float2 mousePosition(App->GetInput()->GetGlobalMousePosition());
    Ray ray = Physics::ScreenPointToRay(mousePosition);
    Plane plane = Plane(mGameObject->GetWorldPosition(), float3::unitY);
    
    float distance;
    bool intersects = plane.Intersects(ray, &distance);
    float3 hitPoint = ray.GetPoint(distance);
    if (intersects)
    {
        float3 target = float3(hitPoint.x, mGameObject->GetWorldPosition().y, hitPoint.z);
        mGameObject->LookAt(target);
    }
}

void PlayerController::Dash()
{
    if (!mIsDashing)
    {
        // Start dashing
        mIsDashing = true;
        mDashTimer = 0.0f;

    }else
    {
        mDashTimer += App->GetDt();

        if (mDashTimer >= mDashDuration)
        {
            //Finish dashing
            mIsDashing = false;
            mCurrentState = PlayerState::IDLE; 
            mDashTimer = 0.0f;
            mMoveDirection = float3::zero;
            mIsDashCoolDownActive = true;
        }
        else 
        {
            // Continue dashing
            float dashSpeed = mDashRange / mDashDuration;
            float3 newFuturePos = (mGameObject->GetPosition() + mMoveDirection * dashSpeed * App->GetDt());
            float3 currentPos = mGameObject->GetPosition();
            float3 navigationPos = App->GetNavigation()->FindNearestPoint(newFuturePos, float3(0.5f));
            if (newFuturePos.x != navigationPos.x && newFuturePos.z != currentPos.z)
            {
                mCurrentState = PlayerState::MOVE;
                return;
            }
            mGameObject->SetPosition(navigationPos);
        }
    }
}

void PlayerController::Attack()
{
    if (mGrenadeAimAreaGO && mGrenadeExplotionPreviewAreaGO)
    {
        if (mAimingGrenade && !mThrowAwayGrenade) {
            GrenadeAttack();
        }
        return;
    }
    else 
    {
        switch (mWeapon)
        {
        case WeaponType::RANGE:
            mAnimationComponent->SendTrigger("tShooting", 0.2f);
            RangedAttack();
            break;
        case WeaponType::MELEE:
            MeleeAttack();
            break;
        }
    }


}

void PlayerController::MeleeAttack()  
{
    if (mLeftMouseButtonPressed) 
    {
        MeleeBaseCombo();
    }
    else 
    {
		MeleeSpecialCombo();
	}
}

void PlayerController::MeleeBaseCombo()
{
    mMeleeBaseComboTimer = 0.0f;
    mIsMeleeBaseComboActive = true;

    switch (mMeleeBaseComboStep)
    {
    case 1:
        //TODO: Implement base attack animation move 1
		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep++;
        mCurrentState = PlayerState::IDLE;

        break;

    case 2:
        //TODO: Implement base attack animation move 2
		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep++;
        mCurrentState = PlayerState::IDLE;

        break;

    case 3:
		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseFinalAttackTimer += App->GetDt();
        if (mMeleeBaseFinalAttackTimer >= mMeleeBaseFinalAttackDuration)
        {
			mMeleeBaseComboStep = 1;
			mIsMeleeBaseComboActive = false;
			mMeleeBaseFinalAttackTimer = 0.0f;
            mCurrentState = PlayerState::IDLE;
		}
        else
        {
            //TODO: Implement base attack animation move 3
            float meleeSpeed = mMeleeBaseMoveRange / mMeleeBaseMoveDuration;
            float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * meleeSpeed * App->GetDt());
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
            mCurrentState = PlayerState::ATTACK;
        }

        break;
	}
}

void PlayerController::MeleeSpecialCombo() {

    mIsMeleeSpecialCoolDownActive = true;
    mMeleeSpecialTimer += App->GetDt();
    if (mMeleeSpecialTimer >= mMeleeSpecialAttackDuration)
    {
        mIsMeleeSpecialCoolDownActive = false; 
        mMeleeSpecialTimer = 0.0f;
	    mCurrentState = PlayerState::IDLE;
    }
    else
    {
        //TODO: Implement special attack animation
        mGameObject->SetPosition(mGameObject->GetPosition());
        mGameObject->SetRotation(mGameObject->GetRotation());
        MeleeHit(mMeleeSpecialRange, mMeleeSpecialDamage);
        mCurrentState = PlayerState::ATTACK;        
    }

}

void PlayerController::MeleeHit (float AttackRange, float AttackDamage) {

    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Enemies;

    scene->FindGameObjectsWithTag(scene->GetTagByName("Enemy")->GetID(), Enemies);
    float3 playerPosition = mGameObject->GetPosition();

    // Recorrer el vector de enemigos y comprobar si hay colisión con el jugador
    for (auto enemy : Enemies)
    {
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)enemy->GetComponent(ComponentType::MESHRENDERER);
        float3 enemyPosition = enemy->GetPosition();
        float distanceToEnemy = (enemyPosition - playerPosition).Length();
        float3 enemyToPlayer = (playerPosition - enemyPosition).Normalized();

        // Si el enemigo está frente al jugador y dentro del rango de ataque
        float3 playerFrontNormalized = mGameObject->GetFront().Normalized();
        float dotProduct = enemyToPlayer.Dot(playerFrontNormalized);

        if (distanceToEnemy < AttackRange && dotProduct < 0)
        {
            Enemy* enemyScript = (Enemy*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if(enemyScript){
                enemyScript->TakeDamage(AttackDamage);
                enemyScript->PushBack();
            }
        }
    }
}

void PlayerController::RangedAttack() 
{

    //Shoot(mRangeBaseDamage);
    if (mRangeWeapon)
    {
        
        mRangeWeapon->BasicAttack();
    }
    if (mShootingTimer > 0.2f) {

        mShootingTimer = 0.0f;
        Idle();
    }
    else
    {
        mShootingTimer += App->GetDt();
    }
}

void PlayerController::Shoot(float damage)
{
  /*  //request a bullet from the object pool
    if (mBulletPool)
    {
        bullet = mBulletPool->GetPooledObject();
    }
    

    if (bullet != nullptr)
    {
        bullet->SetEnabled(true);
        bullet->SetPosition(mGameObject->GetPosition() + float3(0.f, 1.0f, 0.f));
        bullet->SetRotation(mGameObject->GetRotation());
    }

    std::map<float, Hit> hits;
    
    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();
    
    float distance = 100.0f;
    hits = Physics::Raycast(&ray);
    
    if (!hits.empty()) 
    {
        for (const std::pair<float, Hit>& hit : hits)
        {
            if (hit.second.mGameObject->GetTag()->GetName() == "Enemy") 
            {
                LOG("Enemy %s has been hit at distance: %f", hits.begin()->second.mGameObject->GetName().c_str(), hits.begin()->first);
    
                Enemy* enemy = reinterpret_cast<Enemy*>(((ScriptComponent*)hit.second.mGameObject->GetComponentInParent(ComponentType::SCRIPT))->GetScriptInstance());
                if (enemy)
                {
                    enemy->TakeDamage(damage);
                }   
            }
        }
    }
    mCurrentState = PlayerState::IDLE;
    */
}

void PlayerController::Reload()
{
    mBullets = mAmmoCapacity;
    LOG("Reloaded!Remaining bullets : %i", mBullets);
}

void PlayerController::ClosestMouseDirection(const float2& mouseState)
{
    int dx = mouseState.x - 960.0;
    int dy = mouseState.y - 540.0;

    if (std::abs(dx) > std::abs(dy)) {
        if (dx > 0) {
            mLookingAt = MouseDirection::RIGHT;
        }
        else {
            mLookingAt = MouseDirection::LEFT;
        }
    }
    else {
        if (dy > 0) {
            mLookingAt = MouseDirection::DOWN;
        }
        else {
            mLookingAt = MouseDirection::UP;
        }
    }

    if (std::abs(dx) > 0.5 * std::abs(dy) && std::abs(dy) > 0.5 * std::abs(dx)) {
        if (dx > 0 && dy > 0) {
            mLookingAt = MouseDirection::DOWN_RIGHT;
        }
        else if (dx > 0 && dy < 0) {
            mLookingAt = MouseDirection::UP_RIGHT;
        }
        else if (dx < 0 && dy > 0) {
            mLookingAt = MouseDirection::DOWN_LEFT;
        }
        else if (dx < 0 && dy < 0) {
            mLookingAt = MouseDirection::UP_LEFT;
        }
    }
    

    
}

void PlayerController::SetMovingDirection(const float3& moveDirection)
{

    bool movingUp = moveDirection.z > 0 && moveDirection.x < 0 && std::abs(moveDirection.z) < std::abs(moveDirection.x);
    bool movingDown = moveDirection.z < 0 && moveDirection.x > 0 && std::abs(moveDirection.z) < std::abs(moveDirection.x);
    bool movingRight = moveDirection.x < 0 && moveDirection.z < 0 && std::abs(moveDirection.x) < std::abs(moveDirection.z);
    bool movingLeft = moveDirection.x > 0 && moveDirection.z > 0 && std::abs(moveDirection.x) < std::abs(moveDirection.z);

    bool movingUpRight = moveDirection.x < 0 && moveDirection.z < 0 && std::abs(moveDirection.x) > std::abs(moveDirection.z);
    bool movingUpLeft = moveDirection.x < 0 && moveDirection.z > 0 && std::abs(moveDirection.x) < std::abs(moveDirection.z);
    bool movingDownRight = moveDirection.x > 0 && moveDirection.z < 0 && std::abs(moveDirection.x) < std::abs(moveDirection.z);
    bool movingDownLeft = moveDirection.x > 0 && moveDirection.z > 0 && std::abs(moveDirection.x) > std::abs(moveDirection.z);

    //LOG("x: %f, z: %f", moveDirection.x, moveDirection.z);
    if (movingDownLeft) {
        mMovingTo = MoveDirection::DOWN_LEFT;
    }
    else if (movingUpLeft) {
        mMovingTo = MoveDirection::UP_LEFT;
    }
    else if (movingDownRight) {
        mMovingTo = MoveDirection::DOWN_RIGHT;
    }
    else if (movingUpRight) {
        mMovingTo = MoveDirection::UP_RIGHT;
    }
    else if (movingUp) {
        mMovingTo = MoveDirection::UP;
    }
    else if (movingDown) {
        mMovingTo = MoveDirection::DOWN;
    }
    else if (movingRight) {
        mMovingTo = MoveDirection::RIGHT;
    }
    else if (movingLeft) {
       
        mMovingTo = MoveDirection::LEFT;
    }
    else {
        mMovingTo = MoveDirection::NOT_MOVING;
    }
}

void PlayerController::RechargeShield(float shield)
{
    if (mShield < mMaxShield)
    {
        mShield += shield;

        if (mShield >= mMaxShield)
        {
            mShield = mMaxShield;
        }
    }

    UpdateShield();
}

void PlayerController::TakeDamage(float damage)
{
    if (!mIsDashing)
    {
        if (!mGodMode)
        {
            if (mShield > 0.0f)
            {
                mShield -= damage;
                mShield = Max(mShield, 0.0f);
            }
            else
            {
                mCurrentState = PlayerState::DEATH;
            }
            UpdateShield();
        }
    }
}

void PlayerController::Death() 
{
    mPlayerIsDead = true;
    mHudController->SetScreen(SCREEN::LOSE, true);
}

bool PlayerController::IsDead() 
{
    return mPlayerIsDead;
}

void PlayerController::UpdateShield() 
{
    if (mHudController == nullptr) return;
    mHudController->SetHealth(mShield / mMaxShield);
}

void PlayerController::CheckDebugOptions() 
{
    if (App->GetInput()->GetKey(Keys::Keys_J) == KeyState::KEY_REPEAT) 
    {
        mGodMode = (mGodMode) ? !mGodMode : mGodMode = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_1) == KeyState::KEY_DOWN)
    {
        mCurrentState = PlayerState::IDLE;
    }
}

void PlayerController::UpdateGrenadeCooldown()
{
    if (mThrowAwayGrenade)
    {
        if (mGrenadeCoolDownTimer <= 0.0f)
        {
            mGrenadeCoolDownTimer = mGrenadeCoolDown;
            mThrowAwayGrenade = false; 
        }

        if (mGrenadeCoolDownTimer > 0.0f)
        {
            mGrenadeCoolDownTimer -= App->GetDt();
            LOG("Grenade cooldown, wait %f seconds", mGrenadeCoolDownTimer);
        }
        else
        {
            mGrenadeCoolDownTimer = 0.0f;
        }
    }
}

void PlayerController::GrenadeAttack()
{
    AimGrenade();

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        mCurrentState = PlayerState::IDLE;
        mAimingGrenade = false;
        mGrenadeAimAreaGO->SetEnabled(false);
        mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    }
}

void PlayerController::AimGrenade()
{
    // Initialize circle
    mGrenadeAimAreaGO->SetEnabled(true);
    mGrenadeAimAreaGO->SetScale(float3(mGrenadThrowDistance, 0.5, mGrenadThrowDistance));
    mGrenadeAimAreaGO->SetPosition(mGameObject->GetPosition());

    GrenadeTarget();
}

void PlayerController::GrenadeTarget()
{
    float2 mousePosition(App->GetInput()->GetGlobalMousePosition());
    Ray ray = Physics::ScreenPointToRay(mousePosition);
    Plane plane = Plane(mGrenadeAimAreaGO->GetWorldPosition(), float3::unitY);

    float distance;
    bool intersects = plane.Intersects(ray, &distance);
    float3 hitPoint = ray.GetPoint(distance);

    // Check if mouse hit inside circle
    // TODO: Check hit with physic
    float3 diff = hitPoint - mGameObject->GetWorldPosition();
    float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    float radiusSquared = mGrenadThrowDistance * mGrenadThrowDistance;

    bool hit = distanceSquared <= radiusSquared;

    if (intersects)
    {
        float3 finalPosition;
        if (hit)
        {
            finalPosition = hitPoint;
        }
        else
        {
            // Project hitPoint to the edge of the circle
            float distanceToEdge = mGrenadThrowDistance / sqrtf(distanceSquared);
            finalPosition = mGameObject->GetWorldPosition() + diff * distanceToEdge;
        }

        mGrenadeExplotionPreviewAreaGO->SetEnabled(true);
        mGrenadeExplotionPreviewAreaGO->SetScale(float3(mGrenade->GetGrenadeRadius(), 0.5f, mGrenade->GetGrenadeRadius()));
        mGrenadeExplotionPreviewAreaGO->SetPosition(float3(finalPosition.x, 0.3f, finalPosition.z));

        if ((App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)) {
            ThrowGrenade(finalPosition);
        }
    }
}

void PlayerController::ThrowGrenade(float3 target)
{
    mCurrentState = PlayerState::IDLE;
    mAimingGrenade = false;
    mGrenadeAimAreaGO->SetEnabled(false);

    mThrowAwayGrenade = true;

    ScriptComponent* script = (ScriptComponent*)mGrenadeExplotionPreviewAreaGO->GetComponent(ComponentType::SCRIPT);
    Grenade* grenade = (Grenade*)script->GetScriptInstance();

    // TODO wait for thow animation time
    grenade->SetDestionation(target);
}

void PlayerController::UpdateBattleSituation()
{
    float hpRate = mShield / mMaxShield;

    if (mCurrentState == PlayerState::DEATH) 
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
    }
}

void PlayerController::Victory()
{
    mVictory = true;
    mHudController->SetScreen(SCREEN::WIN, true);

    if (Delay(mTimeScreen))
    {
        mHudController->SetScreen(SCREEN::WIN, false);
        mLoadingActive = true;
    }
}

void PlayerController::GameOver()
{
    mGameOver = true;
    mHudController->SetScreen(SCREEN::LOSE, true);

    if (Delay(mTimeScreen))
    {
        mHudController->SetScreen(SCREEN::LOSE, false);
        mLoadingActive = true;
    }
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

void PlayerController::Loading()
{
    if (mLoadingActive)
    {
        mHudController->SetScreen(SCREEN::LOAD, true);

        if (Delay(3.0f))
        {
            mLoadingActive = false;
            mHudController->SetScreen(SCREEN::LOAD, false);
            GameManager::GetInstance()->LoadLevel("MainMenu.json");
        }
    }
}

void PlayerController::OnCollisionEnter(CollisionData* collisionData)
{
    LOG("COLLISION WITH: %s", collisionData->collidedWith->GetName().c_str());
}

