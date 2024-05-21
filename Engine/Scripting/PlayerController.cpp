#include "PlayerController.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "Geometry/Plane.h"
#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "AudioSourceComponent.h"
#include "EnemyExplosive.h"
#include "EnemyRobot.h"
#include "SliderComponent.h"
#include "Physics.h"
#include "ObjectPool.h"
#include "GameManager.h"


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
    MEMBER(MemberType::FLOAT, mFireRate);
    MEMBER(MemberType::INT, mAmmoCapacity);
    MEMBER(MemberType::FLOAT, mMinRangeChargeTime);
    MEMBER(MemberType::FLOAT, mMaxRangeChargeTime);
    MEMBER(MemberType::FLOAT, mRangeChargeAttackMultiplier);
    

    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mShieldGO);

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
    if (mGameManagerGO)
    {
        ScriptComponent* script = (ScriptComponent*)mGameManagerGO->GetComponent(ComponentType::SCRIPT);
        mGameManager = (GameManager*)script->GetScriptInstance();
    }

    mBullets = mAmmoCapacity;
    mShield = mMaxShield;
    mSanity = mMaxSanity;

    if (mShieldGO != nullptr) mShieldSlider = static_cast<SliderComponent*>(mShieldGO->GetComponent(ComponentType::SLIDER));

    

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

    // CAMERA
    ModuleScene* scene = App->GetScene();
    mCamera = scene->FindGameObjectWithTag(scene->GetTagByName("MainCamera")->GetID());


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

        //Transitions
        mStateMachine->AddTransition(defaultState, sIdle, idleTrigger);


        mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sIdle, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sIdle, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkForward, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkForward, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sWalkBack, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkBack, sWalkForward, sWalkForward);
        mStateMachine->AddTransition(sWalkBack, sStrafeLeft, strafeLeftTrigger);
        mStateMachine->AddTransition(sWalkBack, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sStrafeLeft, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sWalkForward, sWalkForward);
        mStateMachine->AddTransition(sStrafeLeft, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeLeft, sStrafeRight, strafeRightTrigger);

        mStateMachine->AddTransition(sStrafeRight, sIdle, idleTrigger);
        mStateMachine->AddTransition(sStrafeRight, sWalkForward, sWalkForward);
        mStateMachine->AddTransition(sStrafeRight, sWalkBack, backTrigger);
        mStateMachine->AddTransition(sStrafeRight, sStrafeLeft, strafeLeftTrigger);

        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);

    }

    //END ANIMATION
}

void PlayerController::Update()
{
    CheckDebugOptions();
    UpdateShield();
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
        GameManager::GetInstance()->WinScreen();
    }

    Loading();
}


void PlayerController::Idle()
{

    if (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        mWeapon = (mWeapon == Weapon::RANGE) ? Weapon::MELEE : Weapon::RANGE;
    }
    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN && !mIsDashCoolDownActive)
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
}

void PlayerController::Moving()
{
    mMoveDirection = float3::zero;
    float3 front = mCamera->GetRight().Cross(float3::unitY).Normalized();

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += front;
        if (mAnimationComponent)
        {
            mAnimationComponent->SendTrigger("tWalkForward", 0.1f);
        }

    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= front;
        if (mAnimationComponent)
        {
            mAnimationComponent->SendTrigger("tWalkBack", 0.1f);
        }

    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += float3::unitY.Cross(front);
        if (mAnimationComponent)
        {
            mAnimationComponent->SendTrigger("tStrafeLeft", 0.1f);
        }

    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= float3::unitY.Cross(front);
        if (mAnimationComponent)
        {
            mAnimationComponent->SendTrigger("tStrafeRight", 0.1f);
        }

    }

    if (!mMoveDirection.Equals(float3::zero))
    {
        mMoveDirection.Normalize();
        Move(mMoveDirection);
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
    if (mCurrentState == PlayerState::ATTACK && mIsMeleeSpecialCoolDownActive)
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
            mIsDashCoolDownActive = true;
        }
        else 
        {
            // Continue dashing
            float dashSpeed = mDashRange / mDashDuration;
            float3 newPos = (mGameObject->GetPosition() + mMoveDirection * dashSpeed * App->GetDt());
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        }
    }
}

void PlayerController::Attack()
{
    switch (mWeapon)
    {
    case Weapon::RANGE:
        RangedAttack();
        break;
    case Weapon::MELEE:
        MeleeAttack();
        break;
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

    Shoot(mRangeBaseDamage);
}

void PlayerController::Shoot(float damage)
{
    //request a bullet from the object pool
    bullet = mBulletPool->GetPooledObject();

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
}

void PlayerController::Reload()
{
    mBullets = mAmmoCapacity;
    LOG("Reloaded!Remaining bullets : %i", mBullets);
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
        }
    }
}

void PlayerController::Death() 
{
    mPlayerIsDead = true;
    GameManager::GetInstance()->LoseScreen();
}

bool PlayerController::IsDead() 
{
    return mPlayerIsDead;
}

void PlayerController::UpdateShield() 
{
    if (mShieldSlider) mShieldSlider->SetFillPercent(mShield / mMaxShield);
}

void PlayerController::CheckDebugOptions() 
{
    if (App->GetInput()->GetKey(Keys::Keys_J) == KeyState::KEY_REPEAT) 
    {
        mGodMode = (mGodMode) ? !mGodMode : mGodMode = true;
    }
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
    App->GetScene()->Find("Victory")->SetEnabled(true);

    if (Delay(mTimeScreen))
    {
        App->GetScene()->Find("Victory")->SetEnabled(false);
        mLoadingActive = true;
    }
}

void PlayerController::GameoOver()
{
    mGameOver = true;
    App->GetScene()->Find("Game_Over")->SetEnabled(true);

    if (Delay(mTimeScreen))
    {
        App->GetScene()->Find("Game_Over")->SetEnabled(false);
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
        App->GetScene()->Find("Loading_Screen")->SetEnabled(true);

        if (Delay(3.0f))
        {
            mLoadingActive = false;
            App->GetScene()->Find("Loading_Screen")->SetEnabled(false);
            App->GetScene()->Load("MainMenu.json");
        }
    }
}