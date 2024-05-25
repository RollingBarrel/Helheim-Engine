#include "pch.h"
#include "GameManager.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("Player");
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
    END_CREATE;
}

GameManager* GameManager::mInstance = nullptr;

GameManager* GameManager::GetInstance()
{
    if (mInstance == nullptr) {
        LOG("GameManager instance has not been initialized.");
        throw std::runtime_error("GameManager instance has not been initialized.");
    }
    return mInstance;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

GameManager::~GameManager()
{
    delete mInstance;
}

void GameManager::Start()
{
    mInstance = this;
}

void GameManager::Update()
{
}

void GameManager::LoadLevel(const char* LevelName)
{
    App->GetScene()->Load(LevelName);
}
