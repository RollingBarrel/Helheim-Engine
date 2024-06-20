#include "BattleArea.h"
#include "GameManager.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "EnemySpawner.h"


CREATE(BattleArea)
{
    CLASS(owner);

    SEPARATOR("Spawner");
    MEMBER(MemberType::GAMEOBJECT, mSpawnerGO1);
    MEMBER(MemberType::GAMEOBJECT, mSpawnerGO2);
    MEMBER(MemberType::GAMEOBJECT, mSpawnerGO3);
    MEMBER(MemberType::GAMEOBJECT, mSpawnerGO4);
    MEMBER(MemberType::INT, mMaxSimulNumEnemies);
    MEMBER(MemberType::INT, mTotalNumEnemies);
    END_CREATE;
}

BattleArea::BattleArea(GameObject* owner): Script(owner)
{
}

BattleArea::~BattleArea()
{
}

void BattleArea::Start()
{
    if (mSpawnerGO1)
    {
        mEnemySpawner1 = reinterpret_cast<EnemySpawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO1->GetComponent(ComponentType::SCRIPT)));
    }
    if (mSpawnerGO2)
    {
        mEnemySpawner2 = reinterpret_cast<EnemySpawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO2->GetComponent(ComponentType::SCRIPT)));
    }
    if (mSpawnerGO3)
    {
        mEnemySpawner3 = reinterpret_cast<EnemySpawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO3->GetComponent(ComponentType::SCRIPT)));
    }
    if (mSpawnerGO4)
    {
        mEnemySpawner4 = reinterpret_cast<EnemySpawner*>(reinterpret_cast<ScriptComponent*>(mSpawnerGO4->GetComponent(ComponentType::SCRIPT)));
    }

}

void BattleArea::Update()
{
    if (mIsActive && mTotalNumEnemies>0)
    {
            if (mEnemySpawner1 && mCurrentEnemies < mMaxSimulNumEnemies)
            {
                mEnemySpawner1->Spawn();
                mCurrentEnemies++;
            }
            if (mEnemySpawner2 && mCurrentEnemies < mMaxSimulNumEnemies)
            {
                mEnemySpawner2->Spawn();
                mCurrentEnemies++;
            }
            if (mEnemySpawner3 && mCurrentEnemies < mMaxSimulNumEnemies)
            {
                mEnemySpawner3->Spawn();
                mCurrentEnemies++;
            }
            if (mEnemySpawner4 && mCurrentEnemies < mMaxSimulNumEnemies)
            {
                mEnemySpawner4->Spawn();
                mCurrentEnemies++;
            }
    }
}

void BattleArea::DestroyEnemy()
{
    --mCurrentEnemies;
    --mTotalNumEnemies;
    if (mTotalNumEnemies != 0)
    {
        mIsActive = false;
        return;
    }

    LOG("REMAINING ENEMIES: %i", mTotalNumEnemies);
}


void BattleArea::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetTag().compare("Player") == 0)
    {
        GameManager::GetInstance()->SetActiveBattleArea(this);
        ActivateArea(true);
        LOG("PLAYER COLLISION");
    }
}