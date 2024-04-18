#include "pch.h"
#include "GameManager.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::BOOL, mSplashScreen);
    END_CREATE;
}

GameManager::GameManager(GameObject* owner) : Script(owner)
{
    LOG("GAME MANAGER_CONSTRUCTOR");
}
void GameManager::Update()
{
    LOG("GAME MANAGER_UPDATE");
}

void GameManager::Start()
{
    // Set the target health to 100
    //mTest = 100.0f;

    LOG("GAME MANAGER_START");
}



