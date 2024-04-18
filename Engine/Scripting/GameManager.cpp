#include "pch.h"
#include "GameManager.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("STATS");
    //MEMBER(MemberType::BOOL, mSplashScreen);
    END_CREATE;
}

GameManager::GameManager(GameObject* owner) : Script(owner)
{
}
void GameManager::Update()
{
}

void GameManager::Start()
{
    LOG("GAME MANAGER_START");
}



