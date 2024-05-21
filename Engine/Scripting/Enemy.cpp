#include "Enemy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"

Enemy::Enemy(GameObject* owner) : Script(owner) {}

void Enemy::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = scene->FindGameObjectWithTag(scene->GetTagByName("Player")->GetID());
    mHealth = mMaxHealth;   
}

void Enemy::Update()
{
}

void Enemy::ActivateEnemy() 
{
}

bool Enemy::IsPlayerInRange(float range) 
{
    float distance = 0.0f;
    distance = (mPlayer) ? mGameObject->GetPosition().Distance(mPlayer->GetPosition()) : inf;

    return (distance <= range);
}

void Enemy::TakeDamage(float damage) 
{   
    mHealth -= damage;
    
    //LOG("Enemy Health: %f", mHealth);

    if (mHealth <= 0)
    {
        Death();
    }
}

void Enemy::Death()
{
    mGameObject->SetEnabled(false);
    DropHealth();
}

bool Enemy::Delay(float delay) //Lapse of time for doing some action
{
   static float timePassed = 0.0f;
   timePassed += App->GetDt();

    if (timePassed >= delay)
    {
        timePassed = 0;
        return true;
    }
    else return false;
}

void Enemy::PushBack() 
{
    float3 direction = mGameObject->GetPosition() - mPlayer->GetPosition();
    direction.Normalize();
    mGameObject->SetPosition(mGameObject->GetPosition() + direction * 2.0f);
}

void Enemy::DropHealth()
{
    srand(static_cast<unsigned int>(std::time(nullptr)));
    int randomValue = rand() % 100;

    if (randomValue < mHealthPercent)
    {
        ModuleScene::InstantiatePrefab("Item_Health.prfb");

        float3 enemyPosition = mGameObject->GetPosition();
        float3 healthPosition = float3(enemyPosition.x, 0.25f, enemyPosition.z);

        GameObject* health = App->GetScene()->Find("Item_Health (2)");
        health->SetPosition(healthPosition);

        float3 scale = float3(0.25f, 0.25f, 0.25f);
        health->SetScale(scale);
    }
}