#include "Enemy.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"

//******************************************

//Includes para en srand
#include <cstdlib>
#include <ctime>

//******************************************

Enemy::Enemy(GameObject* owner) : Script(owner) {

    //*******************************************************
    // Inicializa el generador de números aleatorios
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    //*******************************************************
}

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
    
    LOG("Enemy Health: %f", mHealth);

    if (mHealth <= 0)
    {
        Death();
    }
}

void Enemy::Death()
{
    mGameObject->SetEnabled(false);

    //*******************************************
    
    //App->GetScene()->Find("RootNode")->SetEnabled(false);

    //if (ChanceDropping())
    //{
        DropHealth();
    //}

    //*************************************************
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

//***************************************************************************************

bool Enemy::ChanceDropping()
{
    // Genera un número aleatorio entre 0 y 99
    int randomValue = std::rand() % 100;

    // Retorna true si el valor está en el rango 0-19 (20% de probabilidad)
    if (randomValue < 20)
    {
        return true;
    }
}

void Enemy::DropHealth()
{
    LOG("DROP HEALTH");
    App->GetScene()->Find("Health_Item")->SetEnabled(true);
}