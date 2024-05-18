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

    SEPARATOR("MELEE ATTACK");
   // MEMBER(MemberType::FLOAT, mMeleeBaseDamage);

    SEPARATOR("RANGE ATTACK");
    MEMBER(MemberType::FLOAT, mRangeBaseDamage);
    MEMBER(MemberType::FLOAT, mFireRate);
    MEMBER(MemberType::INT, mAmmoCapacity);
    MEMBER(MemberType::FLOAT, mMinRangeChargeTime);
    MEMBER(MemberType::FLOAT, mMaxRangeChargeTime);
    MEMBER(MemberType::FLOAT, mRangeChargeAttackMultiplier);
    
    SEPARATOR("ANIMATION");
    MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);

    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mShieldGO);
;

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

    if (mAnimationComponentHolder) 
    {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);

        //Redefine player animation clips
        mAnimationComponent->SetCurrentClip(0);
        mAnimationComponent->SetStartTime(0.0f);
        mAnimationComponent->SetEndTime(1.9f);
        mAnimationComponent->SetCurrentClip(1);
        mAnimationComponent->SetStartTime(1.9f);
        mAnimationComponent->SetEndTime(2.9f);
           
        //Set to idle
        mAnimationComponent->SetCurrentClip(0);
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
            LOG("Combo timer exceeded max interval, resetting combo");
            mMeleeBaseComboStep = 0;
            mIsMeleeBaseComboActive = false;
            mMeleeBaseComboTimer = 0.0f;
        }
    }

    switch (mCurrentState)
    {
    case PlayerState::IDLE:
        if (!mVictory && !mGameOver)
        {
            Idle();
            if (mAnimationComponent)
            {
                mAnimationComponent->SetCurrentClip(0);
            }
        }
        break;

    case PlayerState::DASH:
        Dash();
        break;

    case PlayerState::MOVE:
        Moving();
        if (mAnimationComponent)
        {
            mAnimationComponent->SetCurrentClip(1);
        }
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
        LOG(mWeapon == Weapon::RANGE ? "Switched to Range" : "Switched to Melee");

    }
    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN && !mIsDashCoolDownActive)
    {
	    mCurrentState = PlayerState::DASH;
    }
    else 
    {
        if (IsMoving())
        {
            mCurrentState = PlayerState::MOVE;
        }
        else
        {
            mCurrentState = PlayerState::IDLE;
        }
        //Check if the player is attacking
        if (mWeapon == Weapon::RANGE)
        {
            if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT
                || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
            {
                mCurrentState = (mCurrentState == PlayerState::MOVE) ? PlayerState::MOVE_ATTACK : PlayerState::ATTACK;
            }
        }else 
        {
            if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN
                || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
            {
                mCurrentState = (mCurrentState == PlayerState::MOVE) ? PlayerState::MOVE_ATTACK : PlayerState::ATTACK;
            }
        }
    }  
}

//is Moving function 
bool PlayerController::IsMoving()
{
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
		return true;
	}
    else
    {
		return false;
	}
}

void PlayerController::Moving()
{
    bool anyKeyPressed = false;
    
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        Move(float3::unitZ);
        mDashDirection = float3::unitZ;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitZ);
        mDashDirection = -float3::unitZ;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        Move(float3::unitX);
        mDashDirection = float3::unitX;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitX);     
        mDashDirection = -float3::unitX;
        anyKeyPressed = true;
    }

    // Hardcoded play-step-sound solution: reproduce every second 
    // TODO play sound according the animation
    if (anyKeyPressed)
    {
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
             float3 newPos = (mGameObject->GetPosition() + mDashDirection * dashSpeed * App->GetDt());
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        }
    }
}

void PlayerController::Attack()
{
    LOG("Attack state, weapon type: %i", static_cast<int>(mWeapon));

    switch (mWeapon)
    {
    case Weapon::RANGE:
        RangedAttack();
        break;
    case Weapon::MELEE:
        MeleeAttack();
        break;
    }

    Idle();
}


void PlayerController::MeleeAttack()  
{
    LOG("MeleeAttack called, combo active: %i", mIsMeleeBaseComboActive);
  //  if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT){
      MeleeBaseCombo();
   // }
}

void PlayerController::MeleeBaseCombo()
{
    mMeleeBaseComboTimer = 0.0f;
    mIsMeleeBaseComboActive = true;
    mMeleeBaseComboStep = (mMeleeBaseComboStep % 3) + 1;

    LOG("MeleeBaseCombo called, current step: %i", mMeleeBaseComboStep);

    switch (mMeleeBaseComboStep)
    {
    case 1:

		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        LOG("Melee Base Combo Step 1");
        break;
    case 2:
		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
		LOG("Melee Base Combo Step 2");
        break;
    case 3:
		MeleeHit(mMeleeBaseRange, mMeleeBaseDamage);
        LOG("Melee Base Combo Step 3");
        //Move the player forward
        float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * 10.0f);
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));

        break;

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
    if (mIsChargedAttack) 
    {
        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
        {
            mChargedTime += App->GetDt();
            LOG("Charged Time: %f ", mChargedTime);
        }
        else if (mChargedTime >= mMinRangeChargeTime)
        {
            mChargedTime = Min(mMaxRangeChargeTime, mChargedTime);
            float totalDamage;

            int bulletCost = static_cast<int>(mChargedTime * mFireRate);
            if (mBullets >= bulletCost)
            {
                totalDamage = mChargedTime * mRangeBaseDamage * mRangeChargeAttackMultiplier;
                Shoot(totalDamage);
                mBullets -= bulletCost;
                
            }
            else
            {
                totalDamage = ((float)mBullets / mFireRate) * mRangeBaseDamage * mRangeChargeAttackMultiplier;
                mBullets = 0;
            }
            mGunfireAudio->PlayOneShot();
            mChargedTime = 0.0f;
            LOG("Charged shot fired. Damage:  %f", totalDamage);
            LOG("Bullets:  %i", mBullets);

        }
        else
        {
            mChargedTime = 0.0f;
        }
    }
    else 
    {
        if (mBullets > 0) 
        {  
            if (mRangeTimer > mFireRate)
            {
                mGunfireAudio->PlayOneShot();
                mRangeTimer = 0.0f;
                Shoot(mRangeBaseDamage);
                mBullets -= static_cast<int>(mFireRate) + 1;
                LOG("Basic shoot fire. Remining Bullets %i", mBullets);             
            }
            mRangeTimer += App->GetDt();
        }
        else
        {
            LOG("Out of bullets! Reload.");
            Reload();
        }
    }
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
}

void PlayerController::Reload()
{
    mBullets = mAmmoCapacity;
    LOG("Reloaded!Remaining bullets : %i", mBullets);
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