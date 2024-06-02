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
    LOG("s");
}

void AudioSourceComponentNew::Enable()
{
}

void AudioSourceComponentNew::Disable()
{
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
