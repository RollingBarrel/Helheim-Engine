#include "GameManager.h"
#include "pch.h"
#include "Keys.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

#include "GameObject.h"
#include "ScriptComponent.h"

#include "AudioManager.h"
#include "HudController.h"
#include "PlayerController.h"
#include "PlayerCamera.h"
#include "Timer.h"
#include "MathFunc.h"

#include "CinematicCamera.h"

CREATE(GameManager)
{
    CLASS(owner);
    MEMBER(MemberType::BOOL, mController);
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
    MEMBER(MemberType::GAMEOBJECT, mPlayerCameraGO);
    MEMBER(MemberType::GAMEOBJECT, mHudControllerGO);
    MEMBER(MemberType::GAMEOBJECT, mAudioManagerGO);
    MEMBER(MemberType::GAMEOBJECT, mCinematicManagerGO);
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
    //float2 gameSize = App->GetWindow()->GetScreenSize();
    //unsigned int xSize = 46 / (2560 / gameSize.x);
    //unsigned int ySize = 46 / (1440 / gameSize.y);
    //App->GetWindow()->SetCursor(674180654, xSize, ySize, xSize/2, ySize/2);

    App->GetWindow()->SetCursor(674180654, 46, 46, 23, 23);

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

    if (mCinematicManagerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mCinematicManagerGO->GetComponent(ComponentType::SCRIPT));
        mCinematicCamera = static_cast<CinematicCamera*>(script->GetScriptInstance());
    }

    if (mFirstTutorial) 
    {
        mFirstTutorial->SetEnabled(true);
        UnlockGrenade(false);
        UnlockUltimate(false);
    }
    mGameTimer = App->GetCurrentClock();

    mPlayerController->ResetEnergy();
}

void GameManager::Update()
{
    if (mLoadLevel)
    {
        if (mLoadTimer.DelayWithoutReset(1.0f)) 
        {
            if (mHudController) mHudController->SetScreen(SCREEN::LOAD, true);
            if (mLoadSecondTimer.Delay(1.0f))
            {
                EndAudio();
                Clean();
                mLoadLevel = false;
                App->GetScene()->Load(mLevelName);
                return;
            }
        }
    }

    HandleAudio();

    if (mStopActive)
    {
        HitStopTime(mHitStopTime);
    }

    if (mCameraLerp)
    {
        BossCameraMovement();
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN || 
        (UsingController() && (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_START) == ButtonState::BUTTON_DOWN)))
    {
        if (!mPlayingCinematic)
        {
            if (!mPaused || mPaused && mPauseScreen) SetPaused(!mPaused, true);
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_N) == KeyState::KEY_DOWN)
    {
        mDialogue = !mDialogue;
        GameManager::GetInstance()->GetHud()->SetDialogue(!mDialogue);
    }

    if (mCinematicCamera)
    {
        mPlayingCinematic = mCinematicCamera->GetPlayingCinematic();
    }

    if (mController != App->GetInput()->isGamepadAvailable())
    {
        mController = App->GetInput()->isGamepadAvailable();
        mHudController->ChangeBindings(mController);
        App->GetWindow()->ShowCursor(!mController);
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
    mPauseScreen = screen;
    mHudController->SetHud(!value);
    if (screen) mHudController->SetScreen(SCREEN::PAUSE, mPaused);
    App->SetPaused(value);

    if (value) App->GetWindow()->SetCursor(152793723);
    else App->GetWindow()->SetCursor(674180654, 46, 46, 23, 23);
}

void GameManager::LoadLevel(const char* LevelName)
{
    mHudController->SetFadein(false);
    mLoadLevel = true;
    mLevelName = LevelName;
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

    mGameOverAudio = GetAudio()->Play(BGM::GAMEOVER);
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

void GameManager::UnlockSecondary()
{
    if (mPlayerController->GetCurrentEnergy() == 0)
    mPlayerController->RechargeBattery(EnergyType::RED);
}

void GameManager::UnlockUltimate(bool unlock)
{
    mPlayerController->UnlockUltimate(unlock);
}

void GameManager::UnlockGrenade(bool unlock)
{
    mPlayerController->UnlockGrenade(unlock);
}

void GameManager::HandleBossAudio(int stage)
{
    if (mBackgroundAudioID == -1 && stage == -1)
    {
        mBackgroundAudioID = mAudioManager->Play(BGM::BOSS);
    }
    if (mIsFightingBoss && mBackgroundAudioID != -1 && stage >= 0)
    {
        if (mLastAudioID != 2 && stage == 2)
        {
            mAudioManager->UpdateParameterValueByName(BGM::BOSS, mBackgroundAudioID, "boss_states", 4);
            mLastAudioID = 2;
        }
        else if (mLastAudioID != 1 && stage == 1)
        {
            mAudioManager->UpdateParameterValueByName(BGM::BOSS, mBackgroundAudioID, "boss_states", 3);
            mLastAudioID = 1;
        }
        else if (mLastAudioID != 0 && stage == 0)
        {
            mAudioManager->UpdateParameterValueByName(BGM::BOSS, mBackgroundAudioID, "boss_states", 2);
            mLastAudioID = 0;
        }
    }
}

void GameManager::RegisterPlayerKill()
{
    mPlayerController->AddKill();
}

void GameManager::PlayPlayerFootStepSound()
{
    std::string sceneName = App->GetScene()->GetName();
    if (sceneName == "Level1Scene" || sceneName == "Level2Scene")
    {
        int randomValue = std::rand() % 4;
        switch (randomValue)
        {
        case 0:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_FLOOR1, mPlayerController->GetPlayerPosition());

            break;
        case 1:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_FLOOR2, mPlayerController->GetPlayerPosition());

            break;
        case 2:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_FLOOR3, mPlayerController->GetPlayerPosition());

            break;
        case 3:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_FLOOR4, mPlayerController->GetPlayerPosition());

            break;
        default:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_FLOOR1, mPlayerController->GetPlayerPosition());

            break;
        }
    }
    else if (sceneName == "Level3Scene")
    {
        int randomValue = std::rand() % 4;
        switch (randomValue)
        {
        case 0:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_METAL1, mPlayerController->GetPlayerPosition());

            break;
        case 1:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_METAL2, mPlayerController->GetPlayerPosition());

            break;
        case 2:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_METAL3, mPlayerController->GetPlayerPosition());

            break;
        case 3:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_METAL4, mPlayerController->GetPlayerPosition());

            break;
        default:
            mAudioManager->PlayOneShot(SFX::PLAYER_FOOTSTEP_METAL1, mPlayerController->GetPlayerPosition());

            break;
        }
    }
}

void GameManager::ActivateBossCamera(float targetDistance)
{
    mCameraLerp = true;
    mBossCameraTarget = targetDistance;
    BossCameraMovement();
}

void GameManager::BossCameraMovement()
{
    float distance = mPlayerCamera->GetDistanceToPlayer();
    float3 rotation = RadToDeg(mPlayerCameraGO->GetLocalEulerAngles());
    float offset = mPlayerCamera->GetOffset();
    float rotationY = rotation.y;

    distance = Lerp(distance, mBossCameraTarget, App->GetDt());
    rotationY = Lerp(rotationY, -90.0f, App->GetDt()*1.3f);
    offset = Lerp(offset, 0.25f, App->GetDt());

    float diffRot = -90.0f - rotationY;
    float diffDis = mBossCameraTarget - distance;
    float diffOff = 0.45 - offset;
    if (diffDis <= 0.01f && diffRot <= -0.01f && diffOff <= 0.001f)
    {
        mCameraLerp = false;
        distance = mBossCameraTarget;
        rotationY = -90.0f;
        offset = 0.25f;
    }

    rotation.y = rotationY;
    mPlayerCamera->SetDistanceToPlayer(distance);
    mPlayerCameraGO->SetWorldRotation(DegToRad(rotation));
    mPlayerCamera->SetOffset(offset);
}

void GameManager::PauseBackgroundAudio(bool pause)
{
    std::string sceneName = App->GetScene()->GetName();
    if (mBackgroundAudioID != -1)
    {
        if (sceneName == "Level1Scene" || sceneName == "TestAudio")
        {
            mAudioManager->Pause(BGM::LEVEL1, mBackgroundAudioID, pause);
        }
        else if (sceneName == "Level2Scene")
        {
            mAudioManager->Pause(BGM::LEVEL2, mBackgroundAudioID, pause);
        }
        else if (sceneName == "Level3Scene")
        {
            mAudioManager->Pause(BGM::BOSS, mBackgroundAudioID, pause);
        }
    }
}

void GameManager::PrepareAudio()
{
    std::string sceneName = App->GetScene()->GetName();

    // Commun Audio
    mAudioManager->AddAudioToASComponent(BGM::GAMEOVER);

    // Player
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_ULTIMATE);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_PISTOL);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_MACHINEGUN);
    mAudioManager->AddAudioToASComponent(SFX::PLAYER_SHOTGUN);

    // Enemy
    mAudioManager->AddAudioToASComponent(SFX::ENEMY_ROBOT_GUNFIRE);
    mAudioManager->AddAudioToASComponent(SFX::BOSS_LASER);


    // Level Specific audio
    if (sceneName == "Level1Scene" || sceneName == "TestAudio")
    {
        mAudioManager->AddAudioToASComponent(BGM::LEVEL1);
    }
    else if (sceneName == "Level2Scene")
    {
        mAudioManager->AddAudioToASComponent(BGM::LEVEL2);
    }
    else if (sceneName == "Level3Scene")
    {
        mAudioManager->AddAudioToASComponent(BGM::BOSS);
        mAudioManager->AddAudioToASComponent(SFX::BOSS_FIRE);
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
    else if (sceneName == "Level3Scene")
    {
        //mBackgroundAudioID = mAudioManager->Play(BGM::BOSS);
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
    if (mBackgroundAudioID != -1)
    {
        std::string sceneName = App->GetScene()->GetName();

        if (sceneName == "Level1Scene" || sceneName == "TestAudio")
        {
            mBackgroundAudioID = mAudioManager->Release(BGM::LEVEL1, mBackgroundAudioID);
        }
        else if (sceneName == "Level2Scene")
        {
            mBackgroundAudioID = mAudioManager->Release(BGM::LEVEL2, mBackgroundAudioID);
        }
        else if (sceneName == "Level3Scene")
        {
            mBackgroundAudioID = mAudioManager->Release(BGM::BOSS, mBackgroundAudioID);
        }
    }

    if (mGameOverAudio != -1)
    {
        mGameOverAudio = mAudioManager->Release(BGM::GAMEOVER, mGameOverAudio);
    }

    mAudioManager->ReleaseAllAudio();
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
    //if (mActiveBattleArea != nullptr && mPlayerController && mPlayerController->GetShieldPercetage() >= 60.0f && mLastAudioID != 1)
    //{
    //    mAudioManager->UpdateParameterValueByName(BGM::LEVEL2, mBackgroundAudioID, "boss_states", 1);
    //    mLastAudioID = 1;
    //}
    //else if (mActiveBattleArea == nullptr && mLastAudioID != 0)
    //{
    //    mAudioManager->UpdateParameterValueByName(BGM::LEVEL2, mBackgroundAudioID, "boss_states", 0);
    //    mLastAudioID = 0;
    //}
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

