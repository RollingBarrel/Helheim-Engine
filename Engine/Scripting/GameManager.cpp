#include "GameManager.h"
#include "pch.h"
#include "Keys.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "AudioManager.h"
#include "HudController.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "Timer.h"

CREATE(GameManager)
{
    CLASS(owner);
    MEMBER(MemberType::BOOL, mController);
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
    MEMBER(MemberType::GAMEOBJECT, mPlayerCameraGO);
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);
    MEMBER(MemberType::GAMEOBJECT, mAudioManagerGO);
    MEMBER(MemberType::GAMEOBJECT, mPoolManager);
    MEMBER(MemberType::GAMEOBJECT, mFirstTutorial);
    MEMBER(MemberType::GAMEOBJECT, mSecondTutorial);
    MEMBER(MemberType::FLOAT, mDefaultHitStopTime);
    END_CREATE;
}

GameManager* GameManager::mInstance = nullptr;

GameManager* GameManager::GetInstance()
{
    if (mInstance == nullptr) 
    {
        LOG("GameManager instance has not been initialized.");
        throw std::runtime_error("GameManager instance has not been initialized.");
    }
    return mInstance;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

GameManager::~GameManager()
{
    Clean();
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

    if (mPlayerCameraGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mPlayerCameraGO->GetComponent(ComponentType::SCRIPT));
        mPlayerCamera = static_cast<PlayerCamera*>(script->GetScriptInstance());
    }
    
    if (mAudioManagerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mAudioManagerGO->GetComponent(ComponentType::SCRIPT));
        mAudioManager = static_cast<AudioManager*>(script->GetScriptInstance());
        StartAudio();
    }

    if (mFirstTutorial) {

        mFirstTutorial->SetEnabled(true);
        UnlockGrenade(false);
        UnlockUltimate(false);
    }
    mGameTimer = App->GetCurrentClock();
}

void GameManager::Update()
{
    if (mLoadLevel)
    {
        mLoadLevel = false;
        App->GetScene()->Load(mLevelName);
        return;
    }

    HandleAudio();

    if (mStopActive)
    {
        HitStopTime(mHitStopTime);
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN || 
        (UsingController() && (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_START) == ButtonState::BUTTON_DOWN)))
    {
        SetPaused(!mPaused, true);
    }
}

void GameManager::Clean()
{
    mPlayerController = nullptr;
    mActiveBattleArea = nullptr;
    mHudController = nullptr;
    mAudioManager = nullptr;
    mPoolManager = nullptr;
    mGameTimer = nullptr;
}

PoolManager* GameManager::GetPoolManager() const 
{ 
    return reinterpret_cast<PoolManager*>(static_cast<ScriptComponent*>(mPoolManager->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
}

bool GameManager::UsingController() const
{
    return App->GetInput()->isGamepadAvailable();
}

void GameManager::SetPaused(bool value, bool screen)
{
    mPaused = value;
    if (screen) mHudController->SetScreen(SCREEN::PAUSE, mPaused);
}

void GameManager::LoadLevel(const char* LevelName)
{
    if (mHudController) mHudController->SetScreen(SCREEN::LOAD, true);
    EndAudio();
    mLoadLevel = true;
    mLevelName = LevelName;
    Clean();
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

void GameManager::HitStopTime(float time)
{
    time = time * 1000;
    mCurrentStopTime = mGameTimer->GetRealTime();
    float delta = mCurrentStopTime - mStopStart;
    if (delta>time) 
    {
        mGameTimer->SetTimeScale(1.0f);
        mStopActive = false;
    }
}

void GameManager::HitStop()
{
    mHitStopTime = mDefaultHitStopTime;
    mStopStart = mGameTimer->GetRealTime();
    mGameTimer->SetTimeScale(0.0f);
    mStopActive = true;
}

void GameManager::HitStop(float duration)
{
    mHitStopTime = duration;
    mStopStart = mGameTimer->GetRealTime();
    mGameTimer->SetTimeScale(0.0f);
    mStopActive = true;
    
}

void GameManager::ActivateSecondTutorial()
{
    if (mSecondTutorial)
    {
        if (mFirstTutorial) mFirstTutorial->SetEnabled(false);
        mSecondTutorial->SetEnabled(true);
    }
}

void GameManager::UnlockUltimate(bool unlock)
{
    mPlayerController->UnlockUltimate(unlock);
}

void GameManager::UnlockGrenade(bool unlock)
{
    mPlayerController->UnlockGrenade(unlock);
}

void GameManager::PrepareAudio()
{
    std::string sceneName = App->GetScene()->GetName();

    // Commun Audio
    // Player
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_PISTOL);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_MACHINEGUN);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_SHOTGUN);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_ULTIMATE);

    // Enemy
    mAudioManager->AddAudioToASComponent(SFX::ENEMY_ROBOT_GUNFIRE);

    // Level Specific audio
    if (sceneName == "Level1Scene" || sceneName == "TestAudio")
    {
        mAudioManager->AddAudioToASComponent(BGM::LEVEL1);
    }
    else if (sceneName == "Level2Scene")
    {
        mAudioManager->AddAudioToASComponent(BGM::LEVEL2);
    }
}

void GameManager::StartAudio()
{
    PrepareAudio();
    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene" ||  sceneName == "TestAudio")
    {
        mBackgroundAudioID = mAudioManager->Play(BGM::LEVEL1);
    }
    else if (sceneName == "Level2Scene")
    {
        mBackgroundAudioID = mAudioManager->Play(BGM::LEVEL2);
    }
}

void GameManager::HandleAudio()
{
    if (mBackgroundAudioID == -1)
    {
        return;
    }

    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene")
    {
        HandleLevel1Audio();
    }
    else if (sceneName == "Level2Scene")
    {
        HandleLevel2Audio();
    }
}

void GameManager::EndAudio()
{
    if (mBackgroundAudioID == -1)
    {
        return;
    }

    std::string sceneName = App->GetScene()->GetName();

    if (sceneName == "Level1Scene" || sceneName == "TestAudio")
    {
        mBackgroundAudioID = mAudioManager->Release(BGM::LEVEL1, mBackgroundAudioID);
    }
    else if (sceneName == "Level2Scene")
    {
        mBackgroundAudioID = mAudioManager->Release(BGM::LEVEL2, mBackgroundAudioID);
    }
}

void GameManager::HandleLevel1Audio()
{
    if (mActiveBattleArea != nullptr && mPlayerController && mPlayerController->GetShieldPercetage() < 60.0 && mLastAudioID != 2)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "States", 2);
        mLastAudioID = 2;
    }
    else if (mActiveBattleArea != nullptr && mPlayerController && mPlayerController->GetShieldPercetage() >= 60.0f && mLastAudioID != 1)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "States", 1);
        mLastAudioID = 1;
    }
    else if (mActiveBattleArea == nullptr && mLastAudioID != 0)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL1, mBackgroundAudioID, "States",0);
        mLastAudioID = 0;
    }
}

void GameManager::HandleLevel2Audio()
{
    if (mActiveBattleArea != nullptr && mPlayerController && mPlayerController->GetShieldPercetage() < 60.0 && mLastAudioID != 2)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL2, mBackgroundAudioID, "States", 2);
        mLastAudioID = 2;
    }
    else if (mActiveBattleArea != nullptr && mPlayerController && mPlayerController->GetShieldPercetage() >= 60.0f && mLastAudioID != 1)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL2, mBackgroundAudioID, "States", 1);
        mLastAudioID = 1;
    }
    else if (mActiveBattleArea == nullptr && mLastAudioID != 0)
    {
        mAudioManager->UpdateParameterValueByName(BGM::LEVEL2, mBackgroundAudioID, "States", 0);
        mLastAudioID = 0;
    }
}

