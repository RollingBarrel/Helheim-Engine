#include "AudioTester.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

#include "AudioManager.h"
#include "GameManager.h"
#include "GameObject.h"

CREATE(AudioTester)
{
	CLASS(owner);
	END_CREATE;
}

AudioTester::AudioTester(GameObject* owner) : Script(owner)
{
	//**
	// 1 = BGM::Background music Level1 (play/pause)
	// 2 = BGM::Background music Level1 - another instance (play/pause)
	// 3 = BGM::Background music mainmenu
    // 
    // left control + 1 2 3 to release
    // 
	// 4 = SFX::Gunfire audio (play/release)
	// AD = Oneshot SFX::Footstep simulaion (play)
	// L/R Arrow = change BGM 1 audio parameter
}

AudioTester::~AudioTester()
{
}

void AudioTester::Start()
{
}


void AudioTester::Update() 
{
    ControlBGM();
    ControlSFX();

    WalkingSimulation();
    BGM1ParameterChange();
}

void AudioTester::ControlBGM()
{
    auto input = App->GetInput();
    bool controlPressed = input->GetKey(Keys::Keys_LCTRL) == KeyState::KEY_REPEAT;

    // Control BGM 1
    if (input->GetKey(Keys::Keys_1) == KeyState::KEY_DOWN) 
    {
        if (controlPressed) 
        {
            if (mBGM1ID != -1) 
            {
                mBGM1ID = GameManager::GetInstance()->GetAudio()->Release(BGM::LEVEL1, mBGM1ID);
                mBGM1Play = false;
            }
        }
        else 
        {
            if (!mBGM1Play) 
            {
                if (mBGM1ID == -1) 
                {
                    mBGM1ID = GameManager::GetInstance()->GetAudio()->Play(BGM::LEVEL1);
                }
                else 
                {
                    GameManager::GetInstance()->GetAudio()->Pause(BGM::LEVEL1, mBGM1ID, false);
                }
                mBGM1Play = true;
            }
            else 
            {
                GameManager::GetInstance()->GetAudio()->Pause(BGM::LEVEL1, mBGM1ID, true);
                mBGM1Play = false;
            }
        }
    }

    // Control BGM 2
    if (input->GetKey(Keys::Keys_2) == KeyState::KEY_DOWN) 
    {
        if (controlPressed) 
        {
            if (mBGM2ID != -1) 
            {
                mBGM2ID = GameManager::GetInstance()->GetAudio()->Release(BGM::LEVEL1, mBGM2ID);
                mBGM2Play = false;
            }
        }
        else
        {
            if (!mBGM2Play)
            {
                if (mBGM2ID == -1) 
                {
                    mBGM2ID = GameManager::GetInstance()->GetAudio()->Play(BGM::LEVEL1);
                }
                else 
                {
                    GameManager::GetInstance()->GetAudio()->Pause(BGM::LEVEL1, mBGM2ID, false);
                }
                mBGM2Play = true;
            }
            else 
            {
                GameManager::GetInstance()->GetAudio()->Pause(BGM::LEVEL1, mBGM2ID, true);
                mBGM2Play = false;
            }
        }
    }

    // Control BGM 3
    if (input->GetKey(Keys::Keys_3) == KeyState::KEY_DOWN)
    {
        if (controlPressed) 
        {
            if (mBGM3ID != -1) 
            {
                mBGM3ID = GameManager::GetInstance()->GetAudio()->Release(BGM::MAINMENU, mBGM3ID);
                mBGM3Play = false;
            }
        }
        else
        {
            if (!mBGM3Play) 
            {
                if (mBGM3ID == -1) 
                {
                    mBGM3ID = GameManager::GetInstance()->GetAudio()->Play(BGM::MAINMENU);
                }
                else
                {
                    GameManager::GetInstance()->GetAudio()->Pause(BGM::MAINMENU, mBGM3ID, false);
                }
                mBGM3Play = true;
            }
            else
            {
                GameManager::GetInstance()->GetAudio()->Pause(BGM::MAINMENU, mBGM3ID, true);
                mBGM3Play = false;
            }
        }
    }
}

void AudioTester::ControlSFX()
{
    auto input = App->GetInput();

    // Control SFX 1
    if (input->GetKey(Keys::Keys_4) == KeyState::KEY_DOWN) 
    {
        if (!mSFX4Play) 
        {
            if (mSFX4sID == -1)
            {
                mSFX4sID = GameManager::GetInstance()->GetAudio()->Play(SFX::GUNFIRE, mSFX4sID, mGameObject->GetWorldPosition());
            }
            else 
            {
                GameManager::GetInstance()->GetAudio()->Play(SFX::GUNFIRE, mSFX4sID, mGameObject->GetWorldPosition());
            }
            mSFX4Play = true;
        }
        else
        {
            mSFX4sID = GameManager::GetInstance()->GetAudio()->Release(SFX::GUNFIRE, mSFX4sID);
            mSFX4Play = false;
        }
    }
}

void AudioTester::WalkingSimulation()
{
    auto input = App->GetInput();

    // Update movement state
    bool isMovingNow = input->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT || input->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT;
    if (isMovingNow)
    {
        mIsMoving = true;
    }
    else 
    {
        mIsMoving = false;
        mStepTimePassed = 0.0f; // Reset step timer when not moving
    }

    // Handle footsteps sound
    if (mIsMoving)
    {
        mStepTimePassed += App->GetDt();
        if (mStepTimePassed >= mStepCoolDown)
        {
            GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_FOOTSTEP, mGameObject->GetWorldPosition());
            mStepTimePassed = 0.0f;
        }
    }
}

void AudioTester::BGM1ParameterChange()
{
    auto input = App->GetInput();
    if (mBGM1ID != -1)
    {
        if (input->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
        {
            GameManager::GetInstance()->GetAudio()->UpdateParameterValueByName(BGM::LEVEL1,mBGM1ID, "Area", 80);
        }
        else if(input->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) 
        {
            GameManager::GetInstance()->GetAudio()->UpdateParameterValueByName(BGM::LEVEL1, mBGM1ID, "Area", 1);
        }
    }
}

