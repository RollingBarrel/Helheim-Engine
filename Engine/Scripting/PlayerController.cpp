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
    MEMBER(MemberType::FLOAT, mPlayerRotationSpeed);
    MEMBER(MemberType::FLOAT, mDashSpeed);
    MEMBER(MemberType::FLOAT, mDashDistance);
    MEMBER(MemberType::FLOAT, mDashCoolDown);
    MEMBER(MemberType::INT, mDashCharges);

    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mWinArea);
    MEMBER(MemberType::GAMEOBJECT, mLoseArea);
    MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);
    END_CREATE;
}

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{
    mCurrentState = PlayerState::Idle;
    mPreviousState = mCurrentState;
}

void PlayerController::Start()
{
    mNavMeshControl = App->GetScene()->GetNavController();

    if (mAnimationComponentHolder) 
    {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }
   
}


void PlayerController::Update()
{
    switch (mCurrentState)
    {
    case PlayerState::IDLE:
        break;
    case PlayerState::DASH:
        Dash();
        break;
    case PlayerState::MOVE:
        Moving();
        break;
    case PlayerState::MELEE:
        MeleeAttack();
        break;
    case PlayerState::RANGE:
        RangedAttack();
        break;
    case PlayerState::MOVE_MELEE:
        break;
    case PlayerState::MOVE_RANGE:
        break;
    case PlayerState::RELOAD:
        Reload();
        break;
    case PlayerState::GRENADE:
        ThrowGrenade();
        break;
    case PlayerState::DEATH:
        Death();
        break;
    }
    
}

//Change actual animation state of the player
void PlayerController::ChangeState(PlayerState newState) 
{
    mPreviousState = mCurrentState;
    mCurrentState = newState;
    StateMachine();
}

void PlayerController::Controls() 
{
    Mouse_Rotation();

    bool anyKeyPressed = false;

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
    {
        ChangeState(PlayerState::MeleeAttack);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        mChargedShotTime += App->GetGameDt();
        if (mChargedShotTime > 5) {
			mIsChargedShot = true;
		}
		else 
        {
			mIsChargedShot = false;
		}
        ChangeState(PlayerState::RangedAttack);
        anyKeyPressed = true;
    }
    else 
    {
        mChargedShotTime = 0;
        mIsChargedShot = false;
    }

    if (App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN)
    {
        ChangeState(PlayerState::Reload);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN) 
    {
        ChangeState(PlayerState::ThrowGrenade);
        anyKeyPressed = true;
    }

    //*******************************************************************************
    // DAMAGE TEST
    if (App->GetInput()->GetKey(Keys::Keys_C) == KeyState::KEY_DOWN) 
    {
        SetPlayerDamage(5);
    }
    //*******************************************************************************

    if (!anyKeyPressed) 
    {
        mIsMoving = false;
        ChangeState(PlayerState::Idle);
    }
}

void PlayerController::Moving()
{

    bool anyKeyPressed = false;

    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_REPEAT && !mStartCounter)
    {
        mCurrentState = PlayerState::Dash;
    }
    else 
    {
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
            Move(-float3::unitX);
            anyKeyPressed = true;
        }

        if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
        {
            Move(float3::unitX);
            anyKeyPressed = true;
        }

        if (anyKeyPressed)
        {
            mCurrentState = PlayerState::Idle;
        }
    }

}

void PlayerController::Forward() 
{
    //LOG("Forward animation");
    Move(mGameObject->GetFront());
}

void PlayerController::Backward() 
{
    //LOG("Backward animation");
    Move(mGameObject->GetFront() * -1);
}

void PlayerController::Left() 
{
    //LOG("Left animation");
    Move(mGameObject->GetRight());
}

void PlayerController::Right() 
{
    //LOG("Right animation");
    Move(mGameObject->GetRight() * -1);
}


void PlayerController::Move(float3 direction) 
{
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetGameDt() * mPlayerSpeed);
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
}

void PlayerController::Mouse_Rotation() 
{
        int mX, mY;
        App->GetInput()->GetMouseMotion(mX, mY);
        float3 rotation = { 0.0f, DegToRad(mX * mPlayerRotationSpeed), 0.0f };
        mGameObject->SetRotation(rotation+ mGameObject->GetRotation());
}

void PlayerController::Dash()
{
    if (mIsDashActive)
    {
        if (mDashCharges >= 0) 
        {
            //LOG("Dash animation");

            if (mDashTrigger == false) {
                mDashCharges -= 1;
             
                /*
                if (mDashCharges == 0) {
                    mDashCharges = 0;
                }
                */
                LOG("Dash Charges:  %i", mDashCharges);

                mDashTrigger = true;
            }

            if (mDashMovement >= mDashDistance)
            {
                mStartCounter = false;
                mDashMovement = 0;
                mIsDashActive = false;
                mDashTrigger = false;
            }
            if (mStartCounter)
            {
                mDashTimePassed += App->GetGameDt();
                if (mDashTimePassed >= mDashCoolDown)
                {
                    mDashTimePassed = 0;
                    mStartCounter = true;
                }
            }
            else
            {
                mDashMovement += mDashSpeed * App->GetGameDt();
                //mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront() * mDashSpeed * App->GetGameDt());
                float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mDashSpeed);
                mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
            }
        }
    }
}

/*
void PlayerController::Dash()
{
    if (mIsDashActive)
    {
        LOG("Dash animation");

        if (mDashMovement >= mDashDistance)
        {
            mStartCounter = false;
            mDashMovement = 0;
            mIsDashActive = false;
        }
        if (mStartCounter)
        {
            mDashTimePassed += App->GetGameDt();
            if (mDashTimePassed >= mDashCoolDown)
            {
                mDashTimePassed = 0;
                mStartCounter = true;       
            }
        }
        else
        {
            mDashMovement += mDashSpeed * App->GetGameDt();
            //mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront() * mDashSpeed * App->GetGameDt());
            float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mDashSpeed);
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        }
    }
}
*/

void PlayerController::MeleeAttack() 
{
    if (mIsMoving == false) 
    {
        LOG("Melee attack animation");
    }
    else 
    {
        switch (mPreviousState) 
        {
            case PlayerState::Forward:
                LOG("Forward while melee animation");
                break;
            case PlayerState::Backward:
                LOG("Backward while melee animation");
                break;
            case PlayerState::Left:
                LOG("Left while melee animation");
                break;
            case PlayerState::Right:
                LOG("Right while melee animation");
                break;
        }
    }

    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Enemies;

    scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("Enemy")->GetID(), Enemies);
    //player position
    float3 playerPosition = mGameObject->GetPosition();
    // Recorrer el vector de enemigos y comprobar si hay colisión con el jugador
    for (auto enemy : Enemies)
    {
        // Componente de malla del enemigo
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)enemy->GetComponent(ComponentType::MESHRENDERER);

        // Posición del enemigo
        float3 enemyPosition = enemy->GetPosition();
        // Distancia entre el jugador y el enemigo
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
                enemyScript->SetEnemyDamage(mDamage);
            }
            else {
                EnemyRobot* enemyScript = (EnemyRobot*)((ScriptComponent*)enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (enemyScript != nullptr) 
                {
                    enemyScript->SetEnemyDamage(mDamage);
                }
            }
        }
    }
}

void PlayerController::RangedAttack() {
    if (mIsMoving == false) {
        LOG("Range attack animation");
    }
    else
    {
        switch (mPreviousState) {
        case PlayerState::Forward:
            LOG("Forward while ranged animation");
            break;
        case PlayerState::Backward:
            LOG("Backward while ranged animation");
            break;
        case PlayerState::Left:
            LOG("Left while ranged animation");
            break;
        case PlayerState::Right:
            LOG("Right while ranged animation");
            break;
        }
    }

    Shoot(mIsChargedShot, mChargedShotTime);

}


void PlayerController::Shoot(bool isChargedShot, float chargeTime)
{
    if (isChargedShot) {
        //definir que el maximo del tiempo de carga es 20 segundos y el minimo 5 segundos
        if (chargeTime > 20) chargeTime = 20;

        int mDamage = chargeTime * 5;
        int bulletCost = chargeTime * mBulletCostPerSecond;
        if (mBullets >= bulletCost) {
            ShootLogic(mDamage);
            mBullets -= bulletCost;
            LOG("Charged shot fired. Damage:  %i", mDamage);
            LOG("Bullets:  %i", mBullets);
        }
        else {
            LOG("Not enough bullets to fire charged shot.");
        }
    }
    else {
        if (mBullets > 0) {
            ShootLogic(mDamage);
            mBullets--;
            if (mBullets == 0) {
                // Lógica para entrar en el estado de recarga
                LOG("Out of bullets! Cannot shoot.");
            }
            else {
                // Lógica para regresar al estado de reposo
                LOG("Basic shoot fire. Remining Bullets %i", mBullets);
            }
        }
        else {
            LOG("Out of bullets! Cannot shoot.");
        }
    }
}


void PlayerController::ShootLogic(int damage)
{
    std::map<float, GameObject*> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);

    Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));

    //log the first object hit by the ray
    if (!hits.empty()) {
        LOG("Object %s dhas been hit at distance: %f", hits.begin()->second->GetName().c_str(), hits.begin()->first);
        //recorrer todos los hits y hacer daño a los objetos que tengan tag = target
        for (auto hit : hits) {
            if (hit.second->GetTag()->GetName() == "Enemy") {

                EnemyExplosive* enemyScript = (EnemyExplosive*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (enemyScript != nullptr) {
					enemyScript->SetEnemyDamage(damage);
				}
                else {
					EnemyRobot* enemyScript = (EnemyRobot*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                    if (enemyScript != nullptr) {
						enemyScript->SetEnemyDamage(damage);
					}
                }
            }
        }
    }
}

void PlayerController::Reload()
{
    mBullets = mMaxBullets;
    LOG("Reloaded!Remaining bullets : %i", mBullets);
}


void PlayerController::ReloadWeapon()
{
    if (mIsMoving == false) {
        LOG("Reloading animation");
    }
    else {
        switch (mPreviousState) {
        case PlayerState::Forward:
            LOG("Forward while reloading animation");
            break;
        case PlayerState::Backward:
            LOG("Backward while reloading animation");
            break;
        case PlayerState::Left:
            LOG("Left while reloading animation");
            break;
        case PlayerState::Right:
            LOG("Right while reloading animation");
            break;
        }
    }

}

void PlayerController::ThrowGrenade() {
    if (mIsMoving == false) {
        LOG("Throw Grenade animation");
    }
    else {
        switch (mPreviousState) {
            case PlayerState::Forward:
                LOG("Forward while throwing grenade animation");
                break;
            case PlayerState::Backward:
                LOG("Backward while throwing grenade animation");
                break;
            case PlayerState::Left:
                LOG("Left while throwing grenade animation");
                break;
            case PlayerState::Right:
                LOG("Right while throwing grenade animation");
                break;
        }
    }

    //CREATE GRENADE PROJECTILE CODE (If hits enemy = explode, if not, explode after some seconds)

}

void PlayerController::SetPlayerDamage(int damage) {
    if (mIsDashActive == false) {
        if (ShieldDamage(damage) == 0) {
            if (mHealth > 0) {
                mHealth -= damage;
            }
            else {
                mHealth = 0;
                ChangeState(PlayerState::Death);
            }
        }
    } 
}

bool PlayerController::ShieldDamage(int damage) {
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

void PlayerController::Sanity() {

}

void PlayerController::Death() {
    mPlayerIsDead = true;
    LOG("Death animation");
   
    LoseMessage();
}

//Returns if player is dead for the Game Manager
bool PlayerController::GetPlayerIsDead() {
    return mPlayerIsDead;
}

//PROBABLY THIS MESSAGES IS BETTER TO MANAGE IN GAME MANAGER
void PlayerController::WinMessage() {
    LOG("YOU WIN");
}

void PlayerController::LoseMessage() {
    LOG("YOU LOSE");
}

void PlayerController::CheckRoute(){
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
}

//*************************************************************
//ONLY FOR TEST
void PlayerController::WinTest() {
    if (mWinArea) {
        float3 winPosition = mWinArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();

        float distance = winPosition.Distance(playerPosition);
        if (distance < 3.0f) {
            LOG("You WIN");
        }
    }
}

void PlayerController::LoseTest() {
    if (mWinArea) {
        float3 losePosition = mLoseArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();
        if (losePosition.Distance(playerPosition) < 3.0f)
        {
            LOG("You have Lost");
        }
    }
}