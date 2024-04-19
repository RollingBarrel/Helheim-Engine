#include "pch.h"
#include "PlayerController.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "AnimationComponent.h"
#include "EnemyExplosive.h"
#include "EnemyRobot.h"

CREATE(PlayerController)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::INT, mHealth);
    MEMBER(MemberType::INT, mShield);
    MEMBER(MemberType::INT, mSanity);
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
    if (mAnimationComponentHolder) 
    {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }
   
}


void PlayerController::Update()
{

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
        mGameObject->SetRotation(float3(0.0f, DegToRad(0), 0.0f));
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitZ);
        mGameObject->SetRotation(float3(0.0f, DegToRad(180.0f), 0.0f));
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        Move(float3::unitX);
        mGameObject->SetRotation(float3(0.0f, DegToRad(90.0f), 0.0f));
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        Move(-float3::unitX);
        mGameObject->SetRotation(float3(0.0f, DegToRad(-90.0f), 0.0f));
        anyKeyPressed = true;
    }

    Idle();
}

void PlayerController::Move(float3 direction) 
{
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetGameDt() * mPlayerSpeed);
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
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
        float3 playerFrontNormalized = mGameObject->GetFront().Normalized();
        float3 enemyToPlayer = (playerPosition - enemyPosition).Normalized();

        // Si el enemigo está frente al jugador y dentro del rango de ataque
        float3 playerFront = float3(mGameObject->GetFront().x, mGameObject->GetFront().y, mGameObject->GetFront().z);
        float dotProduct = enemyToPlayer.Dot(playerFrontNormalized);

        if (distanceToEnemy < 2.0f && dotProduct < 0)
        {

            EnemyExplosive* enemyScript = (EnemyExplosive*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (enemyScript != nullptr) 
            {
                enemyScript->SetEnemyDamage(mMeleeBaseDamage);
            }
            else 
            {
                EnemyRobot* enemyScript = (EnemyRobot*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (enemyScript != nullptr) 
                {
                    enemyScript->SetEnemyDamage(mMeleeBaseDamage);
                }
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
            mChargedTime += App->GetGameDt();
            LOG("Charged Time: %f ", mChargedTime);
        }
        else if (mChargedTime >= mMinRangeChargeTime)
        {
            mChargedTime = Min(mMaxRangeChargeTime, mChargedTime);
            float totalDamage;

            int bulletCost = mChargedTime * mFireRate;
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
            Shoot(mRangeBaseDamage * App->GetGameDt());
            mBullets-=mFireRate * App->GetGameDt();
            if (mBullets == 0) 
            {
                LOG("Out of bullets! Cannot shoot.");
                Reload();
            }
            else 
            {
                LOG("Basic shoot fire. Remining Bullets %i", mBullets);
            }
        }
        else
        {
            LOG("Out of bullets! Cannot shoot2.");
            Reload();
        }
    }
}


void PlayerController::Shoot(float damage)
{
    std::map<float, GameObject*> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);

    Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));

    if (!hits.empty()) 
    {
        //LOG("Object %s dhas been hit at distance: %f", hits.begin()->second->GetName().c_str(), hits.begin()->first);
        for (auto hit : hits) 
        {
            if (hit.second->GetTag()->GetName() == "Enemy") 
            {
                LOG("Enemy %s has been hit at distance: %f", hits.begin()->second->GetName().c_str(), hits.begin()->first);

                

                EnemyExplosive* enemyScript = (EnemyExplosive*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (enemyScript != nullptr) 
                {
					enemyScript->SetEnemyDamage(damage);
				}
                else 
                {
					EnemyRobot* enemyScript = (EnemyRobot*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                    if (enemyScript != nullptr) 
                    {
						enemyScript->SetEnemyDamage(damage);
					}
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


void PlayerController::SetPlayerDamage(int damage) 
{
    if (mDashMovement == 0) {
        if (ShieldDamage(damage) == 0) {
            if (mHealth > 0) {
                mHealth -= damage;
            }
            else {
                mHealth = 0;
                mCurrentState = PlayerState::DEATH;
            }
        }
    } 
}

bool PlayerController::ShieldDamage(int damage) 
{
    if (mShield > 0) {
        mShield -= damage;
    }
    else {
        mShield = 0;
    }
    
    LOG("Shield: %u", mShield);
    LOG("Player Health: %u", mHealth);
    
    return mShield;
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
}

void PlayerController::Death() 
{
    mPlayerIsDead = true;
    LOG("Death animation");
}

bool PlayerController::GetPlayerIsDead() 
{
    return mPlayerIsDead;
}

void PlayerController::CheckRoute()
{
    /*
    if (App->GetInput()->GetKey(Keys::Keys_P) == KeyState::KEY_REPEAT)
    {
        float3 winPosition = mWinArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();
        std::vector<float3> pathPoints = App->GetNavigation()->FindNavPath(playerPosition, winPosition);

        for (size_t i = 0; i < pathPoints.size() - 1; i++)
        {
            if (i == 0)
            {
                Debug::DrawLine(playerPosition, pathPoints[i], float3(1.0f, 0.0f, 0.0f));
            }
            else if (i < pathPoints.size() - 1)
            {
                Debug::DrawLine(pathPoints[i], pathPoints[i + 1], float3(1.0f, 0.0f, 0.0f));
            }
            else
            {
                Debug::DrawLine(pathPoints[i], winPosition, float3(1.0f, 0.0f, 0.0f));
            }
        }
    }
    */
}
