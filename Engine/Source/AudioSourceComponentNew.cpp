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
    return new AudioSourceComponentNew(*this, owner);
}

void AudioSourceComponentNew::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddInt("ComponentType", static_cast<int>(GetType()));

    std::vector<Archive> audioUnits;
    for (const auto& audio : mAudiosVector) {
        Archive audioArchive;
        audio->Save(audioArchive);
        audioUnits.push_back(std::move(audioArchive));
    }
    archive.AddObjectArray("Audios", audioUnits);
}

void AudioSourceComponentNew::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    if (data.HasMember("Audios") && data["Audios"].IsArray()) {
        for (const auto& item : data["Audios"].GetArray()) {
            AudioUnit* audio = new AudioUnit();
            audio->LoadFromJSON(item);
            mAudiosVector.push_back(audio);
        }
    }
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
        audio->Pause(true);
        audio->Release();
        delete* it;
        mAudiosVector.erase(it);
    }
}
