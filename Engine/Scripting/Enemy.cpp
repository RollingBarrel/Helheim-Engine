#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "AudioSourceComponent.h"
#include "ScriptComponent.h"
#include "GameObject.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "ItemDrop.h"
#include "BattleArea.h"

#include "Math/MathFunc.h"

//Hit Effect
#include "ModuleResource.h"
#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"


Enemy::Enemy(GameObject* owner) : Script(owner) {}

void Enemy::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = GameManager::GetInstance()->GetPlayer();
    mHealth = mMaxHealth;  


    //Hit Effect
    mGameObject->GetComponentsInChildren(ComponentType::MESHRENDERER, mMeshComponents);
    mMaterialIds.reserve(mMeshComponents.size());
    for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    {
        mMaterialIds.push_back(reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->GetResourceMaterial()->GetUID());
    }

}

void Enemy::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    if (mDeath)
    {
        Death();
    }

    //Hit Effect
    //if (mHit)
    //{
    //    if (Delay(0.1f))
    //    {
    //        mHit = false;
    //
    //        for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    //        {
    //            reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->SetMaterial(mMaterialIds[i]);
    //            App->GetResource()->ReleaseResource(mMaterialIds[i]);
    //        }
    //    }
    //}

}

void Enemy::ActivateEnemy() 
{
}

bool Enemy::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

bool Enemy::IsPlayerInRange(float range)
{
    float distance = 0.0f;
    distance = (mPlayer) ? mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition()) : inf;

    return (distance <= range);
}

void Enemy::TakeDamage(float damage) 
{   
    if (mHealth > 0) // TODO: WITHOUT THIS IF DEATH is called two times
    {
        mHealth -= damage;

        if (mHealth <= 0)
        {
            mDeath = true;
        }
    }
        
    

    LOG("Enemy Health: %f", mHealth);

    ////Hit Effect
    //mHit = true;
    //for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
    //{
    //    reinterpret_cast<ResourceMaterial*>(App->GetResource()->RequestResource(mMaterialIds[i], Resource::Type::Material));
    //    reinterpret_cast<MeshRendererComponent*>(mMeshComponents[i])->SetMaterial(999999999);
    //}
}

void Enemy::Death()
{
    mGameObject->SetEnabled(false);

    BattleArea* activeBattleArea = GameManager::GetInstance()->GetActiveBattleArea();
    if (activeBattleArea)
    {
        activeBattleArea->EnemyDestroyed();
    }

    DropItem();
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



void Enemy::PushBack() 
{
    float3 direction = mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition();
    direction.Normalize();
    mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + direction * 2.0f);
}

bool Enemy::IsMoving()
{
    return false;
}

void Enemy::Init()
{
    mDeath = false;
    mHealth = mMaxHealth;
}

void Enemy::DropItem()
{
    srand(static_cast<unsigned int>(std::time(nullptr)));
    int randomValue = rand() % 100;

    PoolType poolType = PoolType::LAST;

    if (randomValue < mShieldDropRate)
    {
        poolType = PoolType::SHIELD;
    }
    else if (randomValue < mRedEnergyDropRate)
    {
        poolType = PoolType::RED_ENERGY;
    }
    else if (randomValue < mBlueEnergyDropRate)
    {
        poolType = PoolType::BLUE_ENERGY;
    }

    if (poolType != PoolType::LAST)
    {
       float3 enemyPosition = mGameObject->GetWorldPosition();
       float3 dropPosition = float3(enemyPosition.x, 0.25f, enemyPosition.z);

       GameObject* itemGameObject = GameManager::GetInstance()->GetPoolManager()->Spawn(poolType);
       itemGameObject->SetWorldPosition(dropPosition);
       ItemDrop* item = reinterpret_cast<ItemDrop*>(reinterpret_cast<ScriptComponent*>(itemGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
       item->Init();
    }


}