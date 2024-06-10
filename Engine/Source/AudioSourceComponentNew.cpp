#include "AudioSourceComponentNew.h"
#include "AudioUnit.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "fmod_studio.hpp"

AudioSourceComponentNew::AudioSourceComponentNew(GameObject* ownerGameObject)
    : Component(ownerGameObject, ComponentType::AUDIOSOURCENEW)
{
}

AudioSourceComponentNew::AudioSourceComponentNew(const AudioSourceComponentNew& original, GameObject* ownerGameObject)
    : Component(ownerGameObject, ComponentType::AUDIOSOURCENEW)
{
}

AudioSourceComponentNew::~AudioSourceComponentNew()
{
}

Component* AudioSourceComponentNew::Clone(GameObject* owner) const
{
    return nullptr;
}

void AudioSourceComponentNew::Save(Archive& archive) const
{
}

void AudioSourceComponentNew::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

void AudioSourceComponentNew::Update()
{
}

void AudioSourceComponentNew::Enable()
{
}

void AudioSourceComponentNew::Disable()
{
}

AudioUnit* AudioSourceComponentNew::FindAudio(const std::string& audioName)
{
    auto it = std::find_if(mAudiosVector.begin(), mAudiosVector.end(),
        [&audioName](const AudioUnit* audioUnit) {
            return audioUnit->GetName() == audioName;
        });
    return (it != mAudiosVector.end()) ? *it : nullptr;
}

void AudioSourceComponentNew::AddNewAudio(FMOD::Studio::EventInstance* event)
{
    AudioUnit* newAudio = new AudioUnit();
    newAudio->SetEventInstance(event);
    mAudiosVector.push_back(newAudio);
}

void AudioSourceComponentNew::AddNewAudioByName(const char* name)
{
    AudioUnit* newAudio = new AudioUnit();
    newAudio->SetEventByName(name);
    mAudiosVector.push_back(newAudio);
}

void AudioSourceComponentNew::RemoveAudio(AudioUnit* audio)
{
    auto it = std::find(mAudiosVector.begin(), mAudiosVector.end(), audio);
    if (it != mAudiosVector.end())
    {
        delete* it;
        mAudiosVector.erase(it);
    }
}
