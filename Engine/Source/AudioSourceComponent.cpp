#include "AudioSourceComponent.h"
#include "AudioUnit.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "fmod_studio.hpp"

AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject)
    : Component(ownerGameObject, ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::AudioSourceComponent(const AudioSourceComponent& original, GameObject* ownerGameObject)
    : Component(ownerGameObject, ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
}

Component* AudioSourceComponent::Clone(GameObject* owner) const
{
    return new AudioSourceComponent(*this, owner);
}

void AudioSourceComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    JsonArray audiosArray = obj.AddNewJsonArray("Audios");
    for (const auto& audio : mAudiosVector) {
        JsonObject audioObj = audiosArray.PushBackNewObject();
        audio->Save(audioObj);
    }
}

void AudioSourceComponent::Load(const JsonObject& obj, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(obj, uidPointerMap);
    if (obj.HasMember("Audios")) {
        JsonArray audiosArray = obj.GetJsonArray("Audios");

        for (unsigned int i = 0; i < audiosArray.Size(); ++i)
        {
            JsonObject audioObj = audiosArray.GetJsonObject(i);
            AudioUnit* audio = new AudioUnit();
            audio->Load(audioObj);
            mAudiosVector.push_back(audio);
        }
    }
}

void AudioSourceComponent::Update()
{
}

void AudioSourceComponent::Enable()
{
}

void AudioSourceComponent::Disable()
{
}

AudioUnit* AudioSourceComponent::FindAudio(const std::string& audioName)
{
    auto it = std::find_if(mAudiosVector.begin(), mAudiosVector.end(),
        [&audioName](const AudioUnit* audioUnit) {
            return audioUnit->GetName() == audioName;
        });
    return (it != mAudiosVector.end()) ? *it : nullptr;
}

void AudioSourceComponent::AddNewAudio(FMOD::Studio::EventInstance* event)
{
    AudioUnit* newAudio = new AudioUnit();
    newAudio->SetEventInstance(event);
    mAudiosVector.push_back(newAudio);
}

void AudioSourceComponent::AddNewAudioByName(const char* name)
{
    AudioUnit* newAudio = new AudioUnit();
    newAudio->SetEventByName(name);
    mAudiosVector.push_back(newAudio);
}

void AudioSourceComponent::RemoveAudio(AudioUnit* audio)
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
