#include "BattleArea.h"


CREATE(BattleArea)
{
    CLASS(owner);

    SEPARATOR("Spawner");
    MEMBER(MemberType::GAMEOBJECT, mSpawner1);
    MEMBER(MemberType::GAMEOBJECT, mSpawner2);
    MEMBER(MemberType::GAMEOBJECT, mSpawner3);
    MEMBER(MemberType::GAMEOBJECT, mSpawner4);
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
}

void BattleArea::Update()
{
    if (mIsActive && mTotalNumEnemies>0)
    {
        if (mCurrentEnemies < mMaxSimulNumEnemies)
        {
            //SPAWN ENEMIES
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

}
