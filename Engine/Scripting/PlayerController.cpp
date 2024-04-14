#include "pch.h"
#include "PlayerController.h"

#include <utility>
#include <limits.h>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "AnimationComponent.h"
#include "Geometry/Ray.h"

#include "Tag.h"
#include "EnemyBase.h"
#include <ScriptComponent.h>

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
    MEMBER(MemberType::FLOAT, mDashLenght);
    MEMBER(MemberType::FLOAT, mDashCoolDown);

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

    if (mAnimationComponentHolder) {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }
}

void PlayerController::Update()
{
    //Sanity();
    ChangeState(PlayerState::Dash);
    Controls();
    CheckRoute();
   
    WinTest();
    LoseTest();

    if (mAnimationComponent) {
        mAnimationComponent->OnUpdate();
    }
}

//Change actual animation state of the player
void PlayerController::ChangeState(PlayerState newState) {
    mPreviousState = mCurrentState;
    mCurrentState = newState;
    StateMachine();
}

//Shows actual animation state of the player
void PlayerController::StateMachine() {
    switch (mCurrentState) {
        case PlayerState::Idle:
            //LOG("Idle animation");
            break;
        case PlayerState::Dash:
            Dash();
            break;
        case PlayerState::Forward:
            Forward();
            break;
        case PlayerState::Backward:
            Backward();
            break;
        case PlayerState::Left:
            Left();
            break;
        case PlayerState::Right:
            Right();
            break;
        case PlayerState::MeleeAttack:
            MeleeAttack();
            break;
        case PlayerState::RangedAttack:
            RangedAttack();
            break;
        case PlayerState::Reload:
            ReloadWeapon();
            break;
        case PlayerState::ThrowGrenade:
            ThrowGrenade();
            break;
        /*
        case PlayerState::Injured:
            Injured();
            break;
        */
        case PlayerState::Death:
            Death();
            break;
        }
}

void PlayerController::Controls() {
    Mouse_Rotation();

    bool anyKeyPressed = false;
    
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
        ChangeState(PlayerState::Forward);
        mIsMoving = true;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
        ChangeState(PlayerState::Backward);
        mIsMoving = true;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
        ChangeState(PlayerState::Left);
        mIsMoving = true;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
        ChangeState(PlayerState::Right);
        mIsMoving = true;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_T) == KeyState::KEY_DOWN && !mStartCounter) {
        mIsDashActive = true;
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN) {
        ChangeState(PlayerState::MeleeAttack);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN) {
        ChangeState(PlayerState::RangedAttack);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN) {
        ChangeState(PlayerState::Reload);
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_G) == KeyState::KEY_DOWN) {
        ChangeState(PlayerState::ThrowGrenade);
        anyKeyPressed = true;
    }

    //*******************************************************************************
    // DAMAGE TEST
    if (App->GetInput()->GetKey(Keys::Keys_C) == KeyState::KEY_DOWN) {
        SetPlayerDamage(5);
    }
    //*******************************************************************************

    if (!anyKeyPressed) {
        mIsMoving = false;
        ChangeState(PlayerState::Idle);
    }
}

void PlayerController::Forward() {
    LOG("Forward animation");
    Move(mGameObject->GetFront());
}

void PlayerController::Backward() {
    LOG("Backward animation");
    Move(mGameObject->GetFront() * -1);
}

void PlayerController::Left() {
    LOG("Left animation");
    Move(mGameObject->GetRight());
}

void PlayerController::Right() {
    LOG("Right animation");
    Move(mGameObject->GetRight() * -1);
}

void PlayerController::Move(float3 direction) {
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetGameDt() * mPlayerSpeed);
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
}

void PlayerController::Mouse_Rotation() {
        int mX, mY;
        App->GetInput()->GetMouseMotion(mX, mY);
        float3 rotation = { 0.0f, DegToRad(mX * mPlayerRotationSpeed), 0.0f };
        mGameObject->SetRotation(rotation+ mGameObject->GetRotation());
}

void PlayerController::Dash()
{
    if (mIsDashActive)
    {
        LOG("Dash animation");

        if (mDashMovement >= mDashLenght)
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

    //MELEE ATTACK CODE
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

}
//función para definir disparar
void PlayerController::Shoot(int damage) {
	//crear un rayo que salga de la posición del player en la dirección del front
	//recorrer todos los objetos que hay en la escena y comprobar si el rayo colisiona con alguno
	//si colisiona, comprobar si el objeto tiene el tag de enemigo
	//si tiene el tag de enemigo, hacer daño al enemigo

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
    }
    
    //recorrer todos los hits y hacer daño a los objetos que tengan tag = target
    for (auto hit : hits) {
        if (hit.second->GetTag()->GetName() =="Enemy"){

            EnemyBase* target = (EnemyBase*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (target != nullptr) {
                target->SetEnemyDamage(damage);
            }
        }
    }
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
    if (ShieldDamage(damage)==0) {
        if (mHealth > 0) {
            mHealth -= damage;
        }
        else {
            mHealth = 0;
            ChangeState(PlayerState::Death);
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
    mPlayerIsDeath = true;
    LOG("Death animation");
   
    LoseMessage();
}

//Returns if player is dead for the Game Manager
bool PlayerController::PlayerIsDeath() {
    return mPlayerIsDeath;
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