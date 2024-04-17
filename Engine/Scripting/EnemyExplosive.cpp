#include "pch.h"
#include "EnemyExplosive.h"

CREATE(EnemyExplosive)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::INT, mHealth);

    MEMBER(MemberType::FLOAT, mEnemySpeed);
    MEMBER(MemberType::FLOAT, mEnemyRotationSpeed);
    MEMBER(MemberType::FLOAT, mActivationDistance);
    MEMBER(MemberType::FLOAT, mArmedDistance);

    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mAnimationComponentHolder);
    //MEMBER(MemberType::GAMEOBJECT, mOpponent);
    END_CREATE;

}

EnemyExplosive::EnemyExplosive(GameObject* owner) : EnemyBase(owner)
{
    mCurrentState = EnemyState::Deploy;
    mHealth = 15;
    mActivationDistance = 10.0f;
    mArmedDistance = 5.0f;
}

void EnemyExplosive::Start()
{
    EnemyBase::Start();
}

void EnemyExplosive::Update()
{
    EnemyBase::Update();
}

//Change actual animation state of the enemy
void EnemyExplosive::ChangeState(EnemyState newState) 
{
    mCurrentState = newState;
    StateMachine();
}

//Shows actual animation state of the enemy
void EnemyExplosive::StateMachine() 
{
    switch (mCurrentState) 
    {
        //******************************************************
        //FOR TEST UNTIL AI WILL BE AVAILABLE (will be changed the system to call the State Machine)
        case EnemyState::Deploy:
            LOG("Deploy animation");
            break;
        case EnemyState::Forward:
            LOG("Forward animation");
            break;
        case EnemyState::Backward:
            LOG("Backward animation");
            break;
        case EnemyState::Left:
            LOG("Left animation");
            break;
        case EnemyState::Right:
            LOG("Right animation");
            break;
        //******************************************************
        case EnemyState::Armed:
            Armed();
            break;
        case EnemyState::Explosion:
            Explosion();
            break;
    }
}

void EnemyExplosive::SearchPlayer() 
{
    EnemyBase::SearchPlayer();

    if (OpponentDistance(mArmedDistance)) {
        mInAttackDistance = true;
        ChangeState(EnemyState::Armed);
    }
}

void EnemyExplosive::SetEnemyDamage(int damage) 
{
    EnemyBase::SetEnemyDamage(damage);

    if (mHealth = 0) 
    {
        ChangeState(EnemyState::Armed);
    }
}

void EnemyExplosive::Armed() 
{  
    LOG("Armed explosive animation");

    if (Delay(5.0f)) 
    {
        ChangeState(EnemyState::Explosion);
    }  
}

void EnemyExplosive::Explosion() 
{
    mInAttackDistance = false;

    //*******************************
    mGameObject->SetEnabled(false);
    //*******************************

    LOG("Explosion animation");

    if (OpponentDistance(mArmedDistance))
    {
        //DAMAGE PLAYER AND OTHER ENEMIES IN DISTANCE RANGE
    }
}

//************************************************************************
//FOR TEST UNTIL AI WILL BE AVAILABLE
void EnemyExplosive::Test_Forward() 
{
    EnemyBase::Test_Forward();
    ChangeState(EnemyState::Forward);
}

void EnemyExplosive::Test_Backward() 
{
    EnemyBase::Test_Backward();
    ChangeState(EnemyState::Backward);
}

void EnemyExplosive::Test_Left() 
{
    EnemyBase::Test_Left();
    ChangeState(EnemyState::Left);
}

void EnemyExplosive::Test_Right() 
{
    EnemyBase::Test_Right();
    ChangeState(EnemyState::Right);
}

//************************************************************************