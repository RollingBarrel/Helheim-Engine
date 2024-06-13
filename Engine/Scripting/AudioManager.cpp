#include "AudioManager.h"
#include "ModuleAudio.h"
#include "AudioUnit.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "GameObject.h"
#include "Component.h"
#include "AudioSourceComponentNew.h"
#include "fmod_studio.hpp"

void AudioManager::Start()
{
}

void AudioManager::Update()
{
}

AudioManager::AudioManager(GameObject* owner) : Script(owner) 
{
    mAudioSources = reinterpret_cast<AudioSourceComponentNew*>(mGameObject->GetComponent(ComponentType::AUDIOSOURCENEW));
}

AudioManager::~AudioManager()
{
}

int AudioManager::PlayBGM(BGM bgm)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return -1;
    }

    int id = App->GetAudio()->Play(description);

    return id;
}

int AudioManager::PlaySFX(SFX sfx)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return -1;
    }

    int id = App->GetAudio()->Play(description);

    return id;
}

void AudioManager::PlayOneShot(SFX sfx)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(sfx);
    if (description == nullptr)
    {
        return;
    }

    int id = App->GetAudio()->Play(description);
    App->GetAudio()->Release(description, id);

    return;
}

void AudioManager::PauseAudio(BGM bgm, int id, bool immediate)
{
    const FMOD::Studio::EventDescription* description = GetEventDescription(bgm);
    if (description == nullptr)
    {
        return;
    }
    App->GetAudio()->Pause(description, id, immediate);

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




