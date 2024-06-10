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

//int AudioManager::Play(const std::string& audioName, float3 audioPosition, const std::unordered_map<std::string, int>& parameters)
//{
//    //AudioUnit* audio = mAudioSources->FindAudio(audioName);
//    //if (audio == nullptr)
//    //{
//    //    return -1;
//    //}
//    //mBackgroundAudioCounter++; 
//    ////FMOD::Studio::EventInstance* newAudioEvent = audio->CreateEventInstance();
//
//    //FMOD::Studio::EventDescription* description = audio->GetDescription();
//
//    //int eventID = App->GetAudio()->Play(description, audioPosition, parameters);
//    
//    // May return description id + audio id
//        // May return description id + audio id
//    // May return description id + audio id
//    // May return description id + audio id
//
//    //mBackgroundAudioMap.insert(std::pair<int, FMOD::Studio::EventInstance*>(mBackgroundAudioCounter, newAudioEvent));
//
//    //App->GetAudio()->StartEvent(newAudioEvent, audioPosition);
//
//    //for (const auto& param : parameters)
//    //{
//    //    const std::string& paramName = param.first;
//    //    float paramValue = param.second;
//
//    //    App->GetAudio()->ChangeEventParameter(newAudioEvent, paramName, paramValue);
//    //}
//
//    //return eventID;
//
//    return 1;
//}

void AudioManager::Play(BGM bgm, int& instanceid)
{
    std::string audioName = GetBGMName(bgm);
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot set audio
        instanceid = -1;
    }

    const FMOD::Studio::EventDescription* description = audio->GetDescription();

    int id = App->GetAudio()->Play(description);

    instanceid = id;
}

void AudioManager::Play(const std::string& audioName, int& instanceid)
{
    const AudioUnit* audio = mAudioSources->FindAudio(audioName);
    if (audio == nullptr)
    {
        // Cannot set audio
        instanceid = -1;
    }

    const FMOD::Studio::EventDescription* description = audio->GetDescription();

    int id = App->GetAudio()->Play(description);

    instanceid = id;
}

void AudioManager::PlayOneShot(const std::string& audioName, float3 audioPosition, const std::unordered_map<std::string, int>& parameters)
{
    //if (!FindAudio(audioName))
    //{
    //    return;
    //}
}

void AudioManager::Release(int audioIndex)
{
    //for (const auto& backGroundAudio : mBackgroundAudioMap)
    //{
    //    if (backGroundAudio.first == audioIndex)
    //    {
    //        App->GetAudio()->ReleaseEvent(backGroundAudio.second);
    //    }
    //}
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




