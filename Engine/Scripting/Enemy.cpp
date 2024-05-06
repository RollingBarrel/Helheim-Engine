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
    mNavMeshControl = App->GetScene()->GetNavController();

    ModuleScene* scene = App->GetScene();
    mPlayer = scene->FindGameObjectWithTag(scene->GetRoot(), scene->GetTagByName("Player")->GetID());
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
    
    LOG("Enemy Health: %f", mHealth);

    if (mHealth <= 0)
    {
        Death();
    }
}

void Enemy::Death()
{
    mGameObject->SetEnabled(false);
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


void Enemy::PushBack() {
    float3 direction = mGameObject->GetPosition() - mPlayer->GetPosition();
    direction.Normalize();
    mGameObject->SetPosition(mGameObject->GetPosition() + direction * 2.0f);
}