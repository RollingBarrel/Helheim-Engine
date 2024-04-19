#include "pch.h"
#include "GameManager.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mTest);
    END_CREATE;
}


//inicialize the target 
GameManager::GameManager(GameObject* owner) : Script(owner)
{

}
void GameManager::Update()
{
}

void GameManager::Start()
{
    // Set the target health to 100
    mTest = 100.0f;
}



