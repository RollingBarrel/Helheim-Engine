#include "pch.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "ScriptComponent.h"
#include "HudController.h"
#include "PlayerController.h"
#include "EnemyPool.h"

CREATE(GameManager)
{
    CLASS(owner);
    MEMBER(MemberType::BOOL, mController);
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);
    MEMBER(MemberType::GAMEOBJECT, mAudioManagerGO);
    MEMBER(MemberType::GAMEOBJECT, mEnemyPool);
    MEMBER(MemberType::GAMEOBJECT, mPoolManager);
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
    assert(!mInstance);
    mInstance = this;
}

void GameManager::Start()
{
    if (mHudControllerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mHudControllerGO->GetComponent(ComponentType::SCRIPT));
        mHudController = static_cast<HudController*>(script->GetScriptInstance());
    }

    if (mPlayer)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mPlayer->GetComponent(ComponentType::SCRIPT));
        mPlayerController = static_cast<PlayerController*>(script->GetScriptInstance());
    }
    
    if (mAudioManagerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mAudioManagerGO->GetComponent(ComponentType::SCRIPT));
        mAudioManager = static_cast<AudioManager*>(script->GetScriptInstance());
    }

    StartAudio();
}

void GameManager::Update()
{
    HandleAudio();
    //App->GetInput()->SetGameControllerRumble(65535, 0, 10);

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        SetPaused(!mPaused);
    }
}

PoolManager* GameManager::GetPoolManager() const 
{ 
    return reinterpret_cast<PoolManager*>(reinterpret_cast<ScriptComponent*>(mPoolManager->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
}

void GameManager::SetPaused(bool value)
{
    mPaused = value;
    mHudController->SetScreen(SCREEN::PAUSE, mPaused);
}

void GameManager::LoadLevel(const char* LevelName)
{
    mHudController->mHealthGradualSlider = nullptr; // TODO: needed?
    App->GetScene()->Load(LevelName);
}

void GameManager::SetActiveBattleArea(BattleArea* activeArea)
{
    mActiveBattleArea = activeArea;
}

void GameManager::Victory()
{
    mPaused = true;

    mHudController->SetScreen(SCREEN::WIN, true);

    EndAudio();
    // Loading activated from HUD controller on Btn Click.
}

void GameManager::GameOver()
{
    mPaused = true;

    mHudController->SetScreen(SCREEN::LOSE, true);

    EndAudio();
    // Loading activated from HUD controller on Btn Click.
}

void GameManager::StartAudio()
{
    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene" || sceneName == "TestAudioWithScene")
    {
        mBackgroundAudioID = mAudioManager->Play(BGM::LEVEL1);
    }
    else if (sceneName == "Level2Scene")
    {
        // TODO
    }
}

void GameManager::HandleAudio()
{
    if (mBackgroundAudioID == -1)
    {
        return;
    }

    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene" || sceneName == "TestAudioWithScene")
    {
        HandleLevel1Audio();
    }
    else if (sceneName == "Level2Scene")
    {
        // TODO
    }
}

void GameManager::EndAudio()
{
    if (mBackgroundAudioID == -1)
    {
        return;
    }

    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene" || sceneName == "TestAudioWithScene")
    {
        mBackgroundAudioID = mAudioManager->Release(BGM::LEVEL1, mBackgroundAudioID);
    }
    else if (sceneName == "Level2Scene")
    {
        // TODO
    }
}

void GameManager::HandleLevel1Audio()
{
    if (mActiveBattleArea != nullptr && mPlayerController->GetShieldPercetage() < 60.0 && mLastAudioID != 80)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "Area", 80);
        mLastAudioID = 80;
    }
    else if (mActiveBattleArea != nullptr && mPlayerController->GetShieldPercetage() >= 60.0f && mLastAudioID != 40)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "Area", 40);
        mLastAudioID = 40;
    }
    else if (mActiveBattleArea == nullptr && mLastAudioID != 1)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "Area", 1);
        mLastAudioID = 1;
    }
}

