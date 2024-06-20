#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameManager.h"
#include "AudioSourceComponent.h"
#include "Math/MathFunc.h"
#include "GameObject.h"





Enemy::Enemy(GameObject* owner) : Script(owner) {}

void Enemy::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = GameManager::GetInstance()->GetPlayer();
    mHealth = mMaxHealth;  
}

void Enemy::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;
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
    if (mHealth > 0)
    {
        mHealth -= damage;

        if (mHealth <= 0)
        {
            Death();
        }
    }

    //LOG("Enemy Health: %f", mHealth);
}

void Enemy::Death()
{
    mGameObject->SetEnabled(false);
    DropShield();
}

void Enemy::AddFootStepAudio(GameObject* audio)
{
    if (mFootstepAudioHolder == nullptr)
    {
        mFootstepAudioHolder = audio;

        if (mFootstepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE) != nullptr)
        {
            AudioSourceComponent* audio = reinterpret_cast<AudioSourceComponent*>(mFootstepAudioHolder->GetComponent(ComponentType::AUDIOSOURCE));
            //audio->Play();
        }
    }
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

bool Enemy::IsMoving()
{
    return false;
}

void Enemy::DropShield()
{
    srand(static_cast<unsigned int>(std::time(nullptr)));
    int randomValue = rand() % 100;

    if (randomValue < mShieldDropRate)
    {
        float3 enemyPosition = mGameObject->GetPosition();
        float3 shieldPosition = float3(enemyPosition.x, 0.25f, enemyPosition.z);

        GameObject* shield = App->GetScene()->InstantiatePrefab("Item_Shield.prfb");
        shield->SetPosition(shieldPosition);

        float3 scale = float3(0.25f, 0.25f, 0.25f);
        shield->SetScale(scale);
    }
}