#include "AudioManager.h"
#include "ModuleAudio.h"
#include "AudioUnit.h"
#include "ScriptComponent.h"
#include "AudioSourceComponent.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "fmod_studio.hpp"

CREATE(AudioManager)
{
    CLASS(owner);
    END_CREATE;
}

void AudioManager::Awake()
{
    mAudioSources = static_cast<AudioSourceComponent*>(mGameObject->GetComponent(ComponentType::AUDIOSOURCE));
}

void AudioManager::Start()
{
}

void AudioManager::Update()
{
}

AudioManager::AudioManager(GameObject* owner) : Script(owner) 
{
}

AudioManager::~AudioManager()
{
}

int AudioManager::Play(BGM bgm, int id, float3 position)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        // Assuming that it is a no-fmod audio
        std::string audioName = GetBGMName(bgm);
        return PlayBGM(audioName);
    }

    if (id != -1)
    {
        // Continue audio
        App->GetAudio()->Play(description, id);
        return id;
    }

    int newid = App->GetAudio()->Play(description);
    SetPosition(description, newid, position);
    return newid;
}

int AudioManager::Play(SFX sfx, int id, float3 position)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        // Assuming that it is a no-fmod audio
        std::string audioName = GetSFXName(sfx);
        return PlaySFX(audioName, position);
    }

    if (id != -1)
    {
        // Continue audio
        App->GetAudio()->Play(description, id);
        return id;
    }

    int newid = App->GetAudio()->Play(description);
    SetPosition(description, newid, position);

    return newid;
}

void AudioManager::PlayOneShot(SFX sfx, float3 position, const std::unordered_map<const char*, float>& parameters)
{
    if (!IsPlayeble(sfx))
    {
        return;
    }

    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        // Assuming that it is a no-fmod audio
        std::string audioName = GetSFXName(sfx);

        PlayOneShot(audioName, position);
        return;
    }

    int newid = App->GetAudio()->Play(description);
    SetPosition(description, newid, position);

    if (parameters.size() > 0)
    {
        for (auto parameter : parameters)
        {
            UpdateParameterValueByName(sfx, newid, parameter.first, parameter.second);
        }
    }

    App->GetAudio()->Release(description, newid);
    return;
}

void AudioManager::Pause(BGM bgm, int id, bool pause)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        Pause(id, pause);
        return;
    }
    App->GetAudio()->Pause(description, id, pause);
    return;
}

void AudioManager::Pause(SFX sfx, int id, bool pause)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        Pause(id, pause);
        return;
    }
    App->GetAudio()->Pause(description, id, pause);

    return;
}

void AudioManager::Restart(SFX sfx, int id)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);

    App->GetAudio()->Restart(description, id);
}

// Stop, kill instance
int AudioManager::Release(BGM bgm, int id)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return Release(id);
    }

    App->GetAudio()->Stop(description, id);
    App->GetAudio()->Release(description, id);

    return -1;
}

int AudioManager::Release(SFX sfx, int id)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return Release(id);
    }
    App->GetAudio()->Stop(description, id);
    App->GetAudio()->Release(description, id);

    return -1;
}

void AudioManager::ReleaseAllAudio()
{
    for (size_t i = 0; i < mStreamAudios.size(); ++i) {
        FMOD::Channel* channel = mStreamAudios[i];
        if (channel) {
            App->GetAudio()->Release(channel);
        }
    }

    App->GetAudio()->ReleaseAllAudio();
}

void AudioManager::UpdateParameterValueByName(BGM bgm, int id, const char* name, const float value)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return;
    }
    App->GetAudio()->UpdateParameter(description, id, name, value);
}

void AudioManager::UpdateParameterValueByName(SFX sfx, int id, const char* name, const float value)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return;
    }
    App->GetAudio()->UpdateParameter(description, id, name, value);
}

void AudioManager::SetPosition(SFX sfx, int id, float3 position)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        SetPosition(id, position);
        return;
    }
    SetPosition(description, id, position);
}

void AudioManager::SetPosition(const FMOD::Studio::EventDescription* description, int id, float3 position)
{
    App->GetAudio()->SetEventPosition(description, id, position);
}

void AudioManager::SetLoop(BGM bgm, int id, bool isLoop)
{
    // Is loop by default
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        FMOD::Channel* channel = mStreamAudios[id];
        App->GetAudio()->SetLoop(channel, isLoop);
        return;
    }
}

void AudioManager::AddAudioToASComponent(BGM bgm)
{
    mAudioSources->AddNewAudioByName(GetBGMName(bgm).c_str());
}

void AudioManager::AddAudioToASComponent(SFX sfx)
{
    mAudioSources->AddNewAudioByName(GetSFXName(sfx).c_str());
}

void AudioManager::AddAudioToASComponent()
{
}

int AudioManager::PlayBGM(const std::string& fileName)
{
    FMOD::Channel* channel = App->GetAudio()->Play(fileName);
    mStreamAudios.push_back(channel);

    return mStreamAudios.size() - 1;
}

int AudioManager::PlaySFX(const std::string& fileName, float3 position)
{
    FMOD::Channel* channel = App->GetAudio()->Play(fileName);
    App->GetAudio()->SetAudioPosition(channel, position);

    mStreamAudios.push_back(channel);

    return mStreamAudios.size() - 1;
}

void AudioManager::Pause(int id, bool state)
{
    FMOD::Channel* channel = mStreamAudios[id];
    App->GetAudio()->Pause(channel, state);
}

void AudioManager::PlayOneShot(const std::string& fileName, float3 eventPosition)
{
    FMOD::Channel* channel = App->GetAudio()->PlayOneShot(fileName);
    App->GetAudio()->SetAudioPosition(channel, eventPosition);
}

int AudioManager::Release(int id)
{
    FMOD::Channel* channel = mStreamAudios[id];
    App->GetAudio()->Release(channel);

    return -1;
}

std::string AudioManager::GetBGMName(BGM bgm)
{
    auto it = mBGMToString.find(bgm);
    if (it != mBGMToString.end()) 
    {
        return it->second;
    }
    return "Unknown";
}

std::string AudioManager::GetSFXName(SFX sfx)
{
    auto it = mSFXToString.find(sfx);
    if (it != mSFXToString.end()) 
    {
        return it->second;
    }
    return "Unknown";
}

void AudioManager::SetPosition(int id, float3 position)
{
    FMOD::Channel* channel = mStreamAudios[id];
    App->GetAudio()->SetAudioPosition(channel, position);
}

const AudioUnit* AudioManager::GetAudioUnit(BGM bgm)
{
    std::string audioName = GetBGMName(bgm);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot get audio
        //LOG("Audio not found");
        return nullptr;
    }
    else 
    {
        return audio;
    }
}

const AudioUnit* AudioManager::GetAudioUnit(SFX sfx)
{
    std::string audioName = GetSFXName(sfx);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot get audio
         //LOG("Audio not found");
        return nullptr;
    }
    else
    {
        return audio;
    }
}

const FMOD::Studio::EventDescription* AudioManager::GetEventDescription(BGM bgm)
{
    std::string audioName = GetBGMName(bgm);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot get audio
        //LOG("Audio not found");
        return nullptr;
    }
    else 
    {
        return audio->GetDescription();
    }
}

const FMOD::Studio::EventDescription* AudioManager::GetEventDescription(SFX sfx)
{
    std::string audioName = GetSFXName(sfx);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot get audio
        //LOG("Audio not found");
        return nullptr;
    }
    else 
    {
        return audio->GetDescription();
    }
}

bool AudioManager::IsPlayeble(SFX sfx)
{
    switch (sfx) 
    {
        case SFX::PLAYER_PISTOL:
        case SFX::PLAYER_SHOTGUN:
        case SFX::PLAYER_MACHINEGUN:
        case SFX::PLAYER_MEELEE:
        case SFX::PLAYER_KATANA:
        case SFX::PLAYER_HAMMER:
        case SFX::ENEMY_ROBOT_GUNFIRE:
        case SFX::ELECTRICAL_TRAP1:
        case SFX::ELECTRICAL_TRAP2:
        case SFX::ELECTRICAL_TRAP3:
        case SFX::BOSS_LASER:
        case SFX::BOSS_FIRE:
            return true;
            break;
    }

    auto currentTime = std::chrono::steady_clock::now();  // Get the current time
    auto lastTimeIt = mLastPlayedTime.find(sfx);

    // If the last play time of this SFX is found, check the time interval
    if (lastTimeIt != mLastPlayedTime.end())
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTimeIt->second).count();
        if (elapsed < 200)  // If the interval is less than 0.2 seconds
        {
            //LOG(" %s is omitted", mSFXToString.at(sfx).c_str());
            // Ignore the play request
            return false;
        }
    }

    // Update the last play time
    mLastPlayedTime[sfx] = currentTime;

    return true;
}



