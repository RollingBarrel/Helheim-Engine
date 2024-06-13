#include "pch.h"
#include "GameManager.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "ScriptComponent.h"
#include "HudController.h"

CREATE(GameManager)
{
    CLASS(owner);
    MEMBER(MemberType::BOOL, mController);
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);
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
    mInstance = nullptr;
}

void GameManager::Awake()
{
    mInstance = this;
}

void GameManager::Start()
{
    if (mHudControllerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mHudControllerGO->GetComponent(ComponentType::SCRIPT));
        mHudController = static_cast<HudController*>(script->GetScriptInstance());
    }
}

void GameManager::Update()
{
    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        mHudController->SetScreen(SCREEN::PAUSE, !mPaused);
    }
}

void GameManager::LoadLevel(const char* LevelName)
{
    mHudController->mHealthGradualSlider = nullptr;
    App->GetScene()->Load(LevelName);
}

void GameManager::Victory()
{
    //TODO: PAUSE GAME

    mHudController->SetScreen(SCREEN::WIN, true);

    // Loading activated from HUD controller on Btn Click.
}

void GameManager::GameOver()
{
    // TODO: PAUSE GAME

    mHudController->SetScreen(SCREEN::LOSE, true);

    // Loading activated from HUD controller on Btn Click.
}
