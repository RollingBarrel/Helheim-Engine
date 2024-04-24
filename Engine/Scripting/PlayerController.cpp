#include "pch.h"
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


CREATE(PlayerController)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mMaxShield);
    MEMBER(MemberType::FLOAT, mMaxSanity);
    MEMBER(MemberType::FLOAT, mPlayerSpeed);

    SEPARATOR("DASH");
    MEMBER(MemberType::FLOAT, mDashSpeed);
    MEMBER(MemberType::FLOAT, mDashDistance);
    MEMBER(MemberType::FLOAT, mDashCoolDown);
    MEMBER(MemberType::INT, mMaxDashCharges);

    SEPARATOR("MELEE ATTACK");
    MEMBER(MemberType::FLOAT, mRangeBaseDamage);
    MEMBER(MemberType::FLOAT, mFireRate);
    MEMBER(MemberType::FLOAT, mMinMeleeChargeTime);
    MEMBER(MemberType::FLOAT, mMaxMeleeChargeTime);
    MEMBER(MemberType::FLOAT, mMeleeChargeAttackMultiplier);

    SEPARATOR("RANGE ATTACK");
    MEMBER(MemberType::FLOAT, mRangeBaseDamage);
    MEMBER(MemberType::INT, mAmmoCapacity);
    MEMBER(MemberType::FLOAT, mMinRangeChargeTime);
    MEMBER(MemberType::FLOAT, mMaxRangeChargeTime);
    MEMBER(MemberType::FLOAT, mRangeChargeAttackMultiplier);
    
    SEPARATOR("ANIMATION");
    MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);

    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHealthGO);
    MEMBER(MemberType::GAMEOBJECT, mDashGO_1);
    MEMBER(MemberType::GAMEOBJECT, mDashGO_2);
    MEMBER(MemberType::GAMEOBJECT, mDashGO_3);

    SEPARATOR("DEBUG MODE");
    MEMBER(MemberType::BOOL, mGodMode);

    SEPARATOR("WIN AREA");
    MEMBER(MemberType::GAMEOBJECT, mWinArea);
    SEPARATOR("AUDIO");
    MEMBER(MemberType::GAMEOBJECT, mFootStepAudioHolder);
    MEMBER(MemberType::GAMEOBJECT, mGunfireAudioHolder);

    END_CREATE;
}

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{
}

void PlayerController::Start()
{
    mDashCharges = mMaxDashCharges;
    mNavMeshControl = App->GetScene()->GetNavController();
    mBullets = mAmmoCapacity;
    mHealth = mMaxHealth;
    mShield = mMaxShield;
    mSanity = mMaxSanity;

    if (mHealthGO != nullptr) mHealthSlider = static_cast<SliderComponent*>(mHealthGO->GetComponent(ComponentType::SLIDER));
    if (mDashGO_1 != nullptr) mDashSlider_1 = static_cast<SliderComponent*>(mDashGO_1->GetComponent(ComponentType::SLIDER));
    if (mDashGO_2 != nullptr) mDashSlider_2 = static_cast<SliderComponent*>(mDashGO_2->GetComponent(ComponentType::SLIDER));
    if (mDashGO_3 != nullptr) mDashSlider_3 = static_cast<SliderComponent*>(mDashGO_3->GetComponent(ComponentType::SLIDER));

    if (mAnimationComponentHolder) 
    {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }

    if (mFootStepAudioHolder)
    {
        mFootStepAudio = (AudioSourceComponent*)mFootStepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mGunfireAudioHolder)
    {
        mGunfireAudio = (AudioSourceComponent*)mGunfireAudioHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }
   
}


void PlayerController::Update()
{
    CheckDebugOptions();
    UpdateHealth();
    UpdateBattleSituation();
    RechargeDash();

    switch (mCurrentState)
    {
    case PlayerState::IDLE:
        Idle();
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

    if (mWinArea)
    {
        if (mGameObject->GetPosition().Distance(mWinArea->GetPosition()) < 2.0f)
        {
            App->GetScene()->Load("MainMenu");
        }
    }


}

void PlayerController::Idle()
{

    if (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        mWeapon = (mWeapon == Weapon::RANGE) ? Weapon::MELEE : Weapon::RANGE;
        
        if (mWeapon == Weapon::RANGE)
        {
            LOG("Range");
        } 
        else 
        {
            LOG("Melee");
        }
    }
    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_REPEAT && mDashCharges > 0)
    {
        mCurrentState = PlayerState::DASH;
    }
    else 
    {

        if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
        {
            mCurrentState = PlayerState::MOVE;
        }
        else
        {
            mCurrentState = PlayerState::IDLE;
        }

        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
        {
            if (mCurrentState == PlayerState::MOVE)
            {
                mCurrentState = PlayerState::MOVE_ATTACK;
            }
            else
            {
                mCurrentState = PlayerState::ATTACK;
            }
        }
    }  
}

void PlayerController::Moving()
{

    bool anyKeyPressed = false;
    
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        Move(float3::unitZ);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitZ);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        Move(float3::unitX);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitX);
        anyKeyPressed = true;
    }

    // Hardcoded play-step-sound solution: reproduce every second 
    // TODO play sound according the animation
    if (anyKeyPressed)
    {
        if (!mReadyToStep)
        {
            mStepTimePassed += App->GetGameDt();
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
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetGameDt() * mPlayerSpeed);
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
}

void PlayerController::HandleRotation()
{
    std::map<float, Hit> hits;

    Ray ray = Physics::ScreenPointToRay(App->GetInput()->GetGameMousePosition());
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
    
    if (mDashMovement >= mDashDistance)
    {
        mIsDashCoolDownActive = false;
        mDashMovement = 0;
        mDashCharges -= 1;
        LOG("Dash Charges:  %i", mDashCharges);
        Idle();
    }
    else 
    {
        if (mIsDashCoolDownActive)
        {
            mDashTimePassed += App->GetGameDt();
            if (mDashTimePassed >= mDashCoolDown)
            {
                mDashTimePassed = 0;
                mIsDashCoolDownActive = true;
            }
        }
        else
        {
            mDashMovement += mDashSpeed * App->GetGameDt();
            float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mDashSpeed);
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        }  
    }
}   

void PlayerController::Attack()
{

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_UP)
    {
        mIsChargedAttack = true;
    }
    else 
    {
        mIsChargedAttack = false;
    }

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
    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Enemies;

    scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("Enemy")->GetID(), Enemies);
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

        if (distanceToEnemy < 2.0f && dotProduct < 0)
        {
            Enemy* enemyScript = (Enemy*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            enemyScript->TakeDamage(mMeleeBaseDamage);
        }
    }
}

void PlayerController::RangedAttack() 
{
    if (mIsChargedAttack) 
    {
        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
        {
            mChargedTime += App->GetGameDt();
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
                mBullets -= mBullets;
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
            static float startingTime = mFireRate;
            startingTime -= App->GetGameDt();

            if (startingTime <= 0.0f)
            {
                mGunfireAudio->PlayOneShot();
                startingTime = mFireRate;
                Shoot(mRangeBaseDamage);
                mBullets -= static_cast<int>(mFireRate);
                LOG("Basic shoot fire. Remining Bullets %i", mBullets);             
            }
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
    std::map<float, Hit> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);
    Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));

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
    if (mDashMovement == 0) 
    {    
        mShield -= damage;
        float remainingDamage = -mShield;
        mShield = Min(mShield, 0.0f);
            
        if (remainingDamage > 0)
        {
            mHealth -= remainingDamage;

            if (mHealth <= 0.0f && !mGodMode)
            {
                mCurrentState = PlayerState::DEATH;
            }
        }    
    } 
}



void PlayerController::RechargeDash()
{

    static float actualRegenerationTime = 0.0f;

    if (mDashCharges < mMaxDashCharges)
    {
        actualRegenerationTime += App->GetGameDt();

        if (actualRegenerationTime >= mDashChargeRegenerationTime)
        {
            mDashCharges++;
            actualRegenerationTime = 0.0f;
            LOG("%i", mDashCharges);
        }
    }

    // HUD
    if (mDashSlider_1 == nullptr || mDashSlider_2 == nullptr || mDashSlider_3 == nullptr) return;
    if (mDashCharges == 0) 
    {
        mDashSlider_1->SetFillPercent(actualRegenerationTime / mDashChargeRegenerationTime);
        mDashSlider_2->SetFillPercent(0);
        mDashSlider_3->SetFillPercent(0);
    }
    else if (mDashCharges == 1) 
    {
        mDashSlider_1->SetFillPercent(1);
        mDashSlider_2->SetFillPercent(actualRegenerationTime/mDashChargeRegenerationTime);
        mDashSlider_3->SetFillPercent(0);
    } 
    else if (mDashCharges == 2) 
    {
        mDashSlider_1->SetFillPercent(1);
        mDashSlider_2->SetFillPercent(1);
        mDashSlider_3->SetFillPercent(actualRegenerationTime / mDashChargeRegenerationTime);
    }
    else 
    {
        mDashSlider_1->SetFillPercent(1);
        mDashSlider_2->SetFillPercent(1);
        mDashSlider_3->SetFillPercent(1);
    }
    
}

void PlayerController::Death() 
{
    mPlayerIsDead = true;
    App->GetScene()->Load("MainMenu");
}

bool PlayerController::IsDead() 
{
    return mPlayerIsDead;
}

void PlayerController::UpdateHealth() 
{
    if (mHealthSlider) mHealthSlider->SetFillPercent(mHealth / mMaxHealth);
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
    float hpRate = mHealth / mMaxHealth;

    if (mCurrentState == PlayerState::DEATH) 
    {
        mCurrentSituation = BattleSituation::DEATH;
    }
    else if ((mPreviousState != PlayerState::ATTACK && mPreviousState != PlayerState::MOVE_ATTACK) &&
        (mCurrentState != PlayerState::ATTACK && mCurrentState != PlayerState::MOVE_ATTACK)) 
    {
        mBattleStateTransitionTime += App->GetGameDt();
        if (mBattleStateTransitionTime >= 8.0f) 
        {
            if (hpRate <= 0.3) {
                mCurrentSituation = BattleSituation::IDLE_LOW_HP;
            }
            else {
                mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
            }
            
        }
    }
    else 
    {
        mBattleStateTransitionTime = 0.0f;

        if (hpRate <= 0.3) {
            mCurrentSituation = BattleSituation::BATTLE_LOW_HP;
        }
        else {
            mCurrentSituation = BattleSituation::BATTLE_HIGHT_HP;
        }
    }
}
