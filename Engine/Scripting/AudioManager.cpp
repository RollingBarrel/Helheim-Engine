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

void AudioManager::Start()
{
    mAudioSources = reinterpret_cast<AudioSourceComponent*>(mGameObject->GetComponent(ComponentType::AUDIOSOURCE));
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
        return -1;
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
        return -1;
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

void AudioManager::PlayOneShot(SFX sfx, float3 position)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return;
    }

    int newid = App->GetAudio()->Play(description);
    SetPosition(description, newid, position);

    App->GetAudio()->Release(description, newid);
    return;
}

void AudioManager::Pause(BGM bgm, int id, bool immediate)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return;
    }
    App->GetAudio()->Pause(description, id, immediate);
    return;
}

void AudioManager::Pause(SFX sfx, int id, bool pause)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return;
    }
    App->GetAudio()->Pause(description, id, pause);

    return;
}

// Stop, kill instance
int AudioManager::Release(BGM bgm, int id)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return id;
    }

    App->GetAudio()->Release(description, id);

    return -1;
}

int AudioManager::Release(SFX sfx, int id)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return id;
    }

    App->GetAudio()->Release(description, id);

    return -1;
}

int AudioManager::PauseAndRelease(BGM bgm, int id)
{
    Pause(bgm, id, true);
    return Release(bgm, id);
}

int AudioManager::PauseAndRelease(SFX sfx, int id)
{
    Pause(sfx, id, true);
    return Release(sfx, id);
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

void AudioManager::SetPosition(const FMOD::Studio::EventDescription* description, int id, float3 position)
{
    App->GetAudio()->SetEventPosition(description, id, position);
}

std::string AudioManager::GetBGMName(BGM bgm)
{
    auto it = mBGMToString.find(bgm);
    if (it != mBGMToString.end()) {
        return it->second;
    }
    return "Unknown";
}

std::string AudioManager::GetSFXName(SFX sfx)
{
    auto it = mSFXToString.find(sfx);
    if (it != mSFXToString.end()) {
        return it->second;
    }
    return "Unknown";
}

const AudioUnit* AudioManager::GetAudioUnit(BGM bgm)
{
    std::string audioName = GetBGMName(bgm);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot get audio
        LOG("Audio not found");
        return nullptr;
    }
    else {
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
        LOG("Audio not found");
        return nullptr;
    }
    else {
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
        LOG("Audio not found");
        return nullptr;
    }
    else {
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
        LOG("Audio not found");
        return nullptr;
    }
    else {
        return audio->GetDescription();
    }
}




