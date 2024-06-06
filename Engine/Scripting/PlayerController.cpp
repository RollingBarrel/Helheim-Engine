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
#include "HudController.h"
#include "Grenade.h"

#include "GameManager.h"
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
    //MEMBER(MemberType::GAMEOBJECT, mRangeWeaponGameObject);

    SEPARATOR("Grenade");
    MEMBER(MemberType::GAMEOBJECT, mGrenadeAimAreaGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeExplotionPreviewAreaGO);
    MEMBER(MemberType::FLOAT, mGrenadeRange);
    MEMBER(MemberType::FLOAT, mGrenadeCoolDown);

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
}

#pragma region Basic Functions

void PlayerController::Start()
{


    mBullets = mAmmoCapacity;
    mShield = mMaxShield;
    mSanity = mMaxSanity;

    //Weapons
    /*if (mRangeWeaponGameObject)
    {
        mRangeWeapon = reinterpret_cast<RangeWeapon*>(reinterpret_cast<ScriptComponent*>(mRangeWeaponGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    }*/

    // AUDIO
    if (mFootStepAudioHolder)
    {
        mFootStepAudio = (AudioSourceComponent*)mFootStepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mGunfireAudioHolder)
    {
        mGunfireAudio = (AudioSourceComponent*)mGunfireAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    // OBJECT POOL
    if (mBulletPoolHolder)
    {
        mBulletPool = (ObjectPool*)((ScriptComponent*)mBulletPoolHolder->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
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


        /*
        mStateMachine->AddTransition(sMeleecombo, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sMeleecombo, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sMeleecombo, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sMeleecombo, sStrafeRight, strafeRightTrigger);
        */
        mStateMachine->AddTransition(sMeleeCombo, sIdle, idleTrigger);


        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);
    }

    //END ANIMATION
}

void PlayerController::Update()
{
    // Check input
    CheckInput();

    // Check state
    StateMachine();

    // Rotate the player to mouse
    HandleRotation();

    // OLD
    CheckDebugOptions();
    UpdateGrenadeCooldown();
    UpdateBattleSituation();


    /*if (mIsMeleeBaseComboActive)
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
    }*/



    // Change for trigger
    if (mWinArea && mGameObject->GetPosition().Distance(mWinArea->GetPosition()) < 2.0f)
    {
        GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::WIN, true);
    }
}

#pragma endregion

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
            case StateType::AIM:
                mLowerState = mAimState;
                break;
            case StateType::ATTACK:
                mLowerState = mAttackState;
                break;
            case StateType::GRENADE:
                mLowerState = mGrenadeState;
                break;
            case StateType::SWITCH:
                mLowerState = mSwitchState;
                break;
            case StateType::SPECIAL:
                mLowerState = mSpecialState;
                break;
            case StateType::RELOAD:
                mLowerState = mReloadState;
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
            case StateType::DASH:
                mUpperState = mDashState;
                break;
            case StateType::MOVE:
                mUpperState = mMoveState;
                break;
            case StateType::IDLE:
                mUpperState = mIdleState;
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
    Plane plane = Plane(mGameObject->GetWorldPosition(), float3::unitY);

    float distance;
    bool intersects = plane.Intersects(ray, &distance);
    float3 hitPoint = ray.GetPoint(distance);

    if (intersects)
    {
        mAimPosition = float3(hitPoint.x, hitPoint.y, hitPoint.z);
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
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(0.5f)));
}

void PlayerController::MoveToPosition(float3 position)
{
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(position, float3(0.5f)));
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

// --------- OLD -----------------
// Done
void PlayerController::Idle() 
{
    /*
    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_REPEAT && !mThrowAwayGrenade)
    {
        mCurrentState = PlayerState::ATTACK;
        mAimingGrenade = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        if (mGrenadeAimAreaGO)
        {
            mGrenadeAimAreaGO->SetEnabled(false);
        }
    }*/
}

void PlayerController::Attack()
{
   
    if (mAimingGrenade && !mThrowAwayGrenade)
    {
        if (mGrenadeAimAreaGO && mGrenadeExplotionPreviewAreaGO)
        {
            GrenadeAttack();
        }
    }
}

void PlayerController::MeleeAttack()
{
    /*
    if (mLeftMouseButtonPressed)
    {
        MeleeBaseCombo();
    }
    else
    {
        MeleeSpecialCombo();
    }
    */
    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        mLeftMouseButtonPressed = true;
    }
    MeleeBaseCombo();
}

void PlayerController::MeleeBaseCombo()
{
    mMeleeBaseComboTimer = 0.0f;
    bool firstHit = false;
    if (!mIsMeleeBaseComboActive)
    {
        // First hit
        mAnimationComponent->SendTrigger("tMelee", 0.2f);
        mIsMeleeBaseComboActive = true;
        mMeleComboCurrentTime = 0.0f;
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep = 2;
        firstHit = true;

    }
    if (mLeftMouseButtonPressed && !mNextComboStep)
    {
        mNextComboStep = !firstHit;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;


    }
    mMeleComboCurrentTime += App->GetDt();
    if (mMeleComboCurrentTime > mMeleeComboMilestone1 && mMeleComboCurrentTime < mMeleeComboMilestone2 && mNextComboStep && mMeleeBaseComboStep == 2)
    {
        // Second hit
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mMeleeBaseComboStep = 3;
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;


    }
    else if (mMeleComboCurrentTime > mMeleeComboMilestone2 && mMeleComboCurrentTime < mMeleeComboDuration && mMeleeBaseComboStep == 3 && mNextComboStep)
    {
        // Dashing
        float meleeSpeed = mMeleeBaseMoveRange / mMeleeBaseMoveDuration;
        float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * meleeSpeed * App->GetDt());
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        mBreakMeleeCombo = 0.0f;

    }
    else if (mMeleComboCurrentTime > mMeleeComboDuration && mMeleeBaseComboStep == 3 && mNextComboStep)
    {
        //End combo
        MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mIsMeleeBaseComboActive = false;
        mMeleComboCurrentTime = 0.0f;
        //mCurrentState = PlayerState::IDLE;
        mMeleeBaseComboStep = 4; //this variable is weird, refactor how it works in next updates
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;

    }
    else if (!mNextComboStep)
    {
        mBreakMeleeCombo += App->GetDt();
    }

    float breakTime = 10.0f;
    switch (mMeleeBaseComboStep)
    {
    case 1:
        breakTime = 10.0f;
        break;
    case 2:
        breakTime = 1.1f;
        break;
    case 3:
        breakTime = 0.9f;
        break;
    default:
        break;
    }

    if (mBreakMeleeCombo > breakTime)
    {
        mAnimationComponent->SendTrigger("tIdle", 0.2f);
        mIsMeleeBaseComboActive = false;
        mMeleComboCurrentTime = 0.0f;
        //mCurrentState = PlayerState::IDLE;
        mMeleeBaseComboStep = 1; //this variable is weird, refactor how it works in next updates
        mNextComboStep = false;
        mLeftMouseButtonPressed = false;
        mBreakMeleeCombo = 0.0f;

    }

    if (mMeleComboCurrentTime > mMeleeComboDuration + 2.0f)
    {
        mMeleComboCurrentTime = 0.0f;
    }
}

void PlayerController::MeleeSpecialCombo() {

    mIsMeleeSpecialCoolDownActive = true;
    mMeleeSpecialTimer += App->GetDt();
    if (mMeleeSpecialTimer >= mMeleeSpecialAttackDuration)
    {
        mIsMeleeSpecialCoolDownActive = false;
        mMeleeSpecialTimer = 0.0f;
        //mCurrentState = PlayerState::IDLE;
    }
    else
    {
        //TODO: Implement special attack animation
        mGameObject->SetPosition(mGameObject->GetPosition());
        mGameObject->SetRotation(mGameObject->GetRotation());
        MeleeHit(mMeleeSpecialRange, mMeleeSpecialDamage);
        //mCurrentState = PlayerState::ATTACK;
    }

}

void PlayerController::MeleeHit(float AttackRange, float AttackDamage) {

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
            if (enemyScript) {
                enemyScript->TakeDamage(AttackDamage);
                enemyScript->PushBack();
            }
        }
    }
}

void PlayerController::ClosestMouseDirection(const float2& mouseState)
{

    float2 window(App->GetWindow()->GetGameWindowsPosition());
    float2 windowSize(App->GetWindow()->GetGameWindowsSize());

    int dx = mouseState.x - (window.x + windowSize.x * 0.5);
    int dy = mouseState.y - (window.y + windowSize.y * 0.5);

    LOG("x: %f, y: %f", mouseState.x, mouseState.y);

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

void PlayerController::CheckDebugOptions()
{
    if (App->GetInput()->GetKey(Keys::Keys_J) == KeyState::KEY_REPEAT)
    {
        mGodMode = (mGodMode) ? !mGodMode : mGodMode = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_1) == KeyState::KEY_DOWN)
    {
        //mCurrentState = PlayerState::IDLE;
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
    if (mIsDashing || mGodMode) 
    {
        return;
    }

    mShield = Clamp(mShield - damage, 0.0f, mMaxShield);
    UpdateShield();

    if (mShield < 0.0f)
    {
        //mCurrentState = PlayerState::DEATH;
    }
}

void PlayerController::UpdateShield()
{
    float healthRatio = mShield / mMaxShield;
    GameManager::GetInstance()->GetHud()->SetHealth(healthRatio);
}

#pragma endregion

#pragma region Grenade

void PlayerController::UpdateGrenadeCooldown()
{
    if (mThrowAwayGrenade)
    {
        if (mGrenadeCoolDownTimer <= 0.0f)
        {
            mGrenadeCoolDownTimer = mGrenadeCoolDown;
            mThrowAwayGrenade = false;
            GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(0.0f);
            return;
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

        GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mGrenadeCoolDownTimer / mGrenadeCoolDown);
    }
}

void PlayerController::GrenadeAttack()
{
    AimGrenade();

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        //mCurrentState = PlayerState::IDLE;
        mAimingGrenade = false;
        mGrenadeAimAreaGO->SetEnabled(false);
        mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    }
}

void PlayerController::AimGrenade()
{
    // Initialize circle
    mGrenadeAimAreaGO->SetEnabled(true);
    mGrenadeAimAreaGO->SetScale(float3(mGrenadeRange, 0.5, mGrenadeRange));
    mGrenadeAimAreaGO->SetPosition(mGameObject->GetPosition());

    GrenadeTarget();
}

void PlayerController::GrenadeTarget()
{
    Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetLocalMousePosition());
    Plane plane = Plane(mGrenadeAimAreaGO->GetWorldPosition(), float3::unitY);

    float distance;
    bool intersects = plane.Intersects(ray, &distance);
    float3 hitPoint = ray.GetPoint(distance);

    // Check if mouse hit inside circle
    // TODO: Check hit with physic
    float3 diff = hitPoint - mGameObject->GetWorldPosition();
    float distanceSquared = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    float radiusSquared = mGrenadeRange * mGrenadeRange;

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
            float distanceToEdge = mGrenadeRange / sqrtf(distanceSquared);
            finalPosition = mGameObject->GetWorldPosition() + diff * distanceToEdge;
        }
        mGrenadeExplotionPreviewAreaGO->GetChildren()[0]->SetEnabled(true);
        mGrenadeExplotionPreviewAreaGO->GetChildren()[1]->SetEnabled(false);

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
    //mCurrentState = PlayerState::IDLE;
    mAimingGrenade = false;
    mGrenadeAimAreaGO->SetEnabled(false);

    mGrenadeExplotionPreviewAreaGO->SetEnabled(false);
    mGrenadeExplotionPreviewAreaGO->SetEnabled(true);

    mGrenadeExplotionPreviewAreaGO->GetChildren()[0]->SetEnabled(false);
    mGrenadeExplotionPreviewAreaGO->GetChildren()[1]->SetEnabled(true);



    mThrowAwayGrenade = true;

    ScriptComponent* script = (ScriptComponent*)mGrenadeExplotionPreviewAreaGO->GetComponent(ComponentType::SCRIPT);
    Grenade* grenade = (Grenade*)script->GetScriptInstance();

    // TODO wait for thow animation time
    grenade->SetDestionation(target);
}

#pragma endregion

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

#pragma region Game State

void PlayerController::Victory()
{
    mVictory = true;
    GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::WIN, true);

    // Loading activated from HUD controller on Btn Click.
}

void PlayerController::GameOver()
{
    mGameOver = true;
    GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::LOSE, true);

    // Loading activated from HUD controller on Btn Click.
}

#pragma endregion

#pragma region Util

bool PlayerController::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    return false;
}

#pragma endregion

void PlayerController::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetName().compare("WinArea") == 0)
    {
        GameManager::GetInstance()->GetHud()->SetScreen(SCREEN::WIN, true);
    }
    LOG("COLLISION WITH: %s", collisionData->collidedWith->GetName().c_str());
}

