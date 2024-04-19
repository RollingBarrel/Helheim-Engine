#include "pch.h"
#include "EnemyBase.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "AnimationComponent.h"

EnemyBase::EnemyBase(GameObject* owner) : Script(owner) {}

void EnemyBase::Start()
{
    mNavMeshControl = App->GetScene()->GetNavController();

    if (mAnimationComponentHolder) 
    {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }
}

void EnemyBase::Update()
{
    ActivateEnemy();

    if (mAnimationComponent) 
    {
        mAnimationComponent->OnUpdate();
    }
}

void EnemyBase::ActivateEnemy() 
{
    if (mEnemyActivated == false) 
    {
        if (OpponentDistance(mActivationDistance)) 
        {
            mEnemyActivated = true;
            LOG("PLAYER IN RANGE. Movement activated");
        }
    }
    
    if (mEnemyActivated) 
    {
        SearchPlayer();
    }
}

//This is the distance from the enemy (self) to an opponent (player or another enemy)
bool EnemyBase::OpponentDistance(float triggerDistance) 
{
    ModuleScene* scene = App->GetScene();
    std::vector<GameObject*> Opponents;

    scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("Player")->GetID(), Opponents);
    
    float3 selfPosition = mGameObject->GetPosition();

    for (auto opponent : Opponents)
    {
        float3 opponentPosition = opponent->GetPosition();

        float distance = selfPosition.Distance(opponentPosition);

        if (distance < triggerDistance) 
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}

void EnemyBase::SearchPlayer() 
{
    if (mInAttackDistance == false) 
    {
        Test_Controls(); //Enemy's test movement emulating AI
    }
}

void EnemyBase::SetEnemyDamage(int damage) 
{
    if (mHealth > 0) 
    {
        mHealth -= damage;
    }
    LOG("Enemy Health: %u", mHealth);
}

bool EnemyBase::Delay(float delay)
{
    mTimePassed += App->GetGameDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

//************************************************************************
//FOR TEST UNTIL AI WILL BE AVAILABLE
void EnemyBase::Test_Controls() 
{
    Test_Mouse_Rotation();

    bool anyKeyPressed = false;

    if (App->GetInput()->GetKey(Keys::Keys_I) == KeyState::KEY_REPEAT) 
    {
        Test_Forward();
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_K) == KeyState::KEY_REPEAT) 
    {
        Test_Backward();
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_J) == KeyState::KEY_REPEAT) 
    {
        Test_Left();
        anyKeyPressed = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_L) == KeyState::KEY_REPEAT) 
    {
        Test_Right();
        anyKeyPressed = true;
    }

    //*******************************************************************************
    // DAMAGE TEST
    if (App->GetInput()->GetKey(Keys::Keys_M) == KeyState::KEY_DOWN) 
    {
        SetEnemyDamage(5);
    }
    //*******************************************************************************

    if (!anyKeyPressed) 
    {
        //ChangeState(EnemyState::Idle);
    }
}

void EnemyBase::Test_Forward() 
{
    Move(mGameObject->GetFront());
}

void EnemyBase::Test_Backward() 
{
    Move(mGameObject->GetFront() * -1);
}

void EnemyBase::Test_Left() 
{
    Move(mGameObject->GetRight());
}

void EnemyBase::Test_Right() 
{
    Move(mGameObject->GetRight() * -1);
}

void EnemyBase::Move(float3 direction) 
{
    float3 newPos = (mGameObject->GetPosition() + direction * App->GetGameDt() * mEnemySpeed);
    mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
}

void EnemyBase::Test_Mouse_Rotation() 
{
    int mX, mY;
    App->GetInput()->GetMouseMotion(mX, mY);
    float3 rotation = { 0.0f, DegToRad(mX * mEnemyRotationSpeed), 0.0f };
    mGameObject->SetRotation(rotation + mGameObject->GetRotation());
}
//************************************************************************