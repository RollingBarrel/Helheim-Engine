#pragma once
#include "Component.h"
#include "float3.h"

#include <vector>
#include <map>

class AudioUnit;

namespace FMOD {
    namespace Studio {
        class EventInstance;
        class EventDescription;
    }
}

class ENGINE_API AudioSourceComponent : public Component
{
public:
    AudioSourceComponent(GameObject* ownerGameObject);
    AudioSourceComponent(const AudioSourceComponent& original, GameObject* owner);
    ~AudioSourceComponent();

    Component* Clone(GameObject* owner) const;

    void Save(JsonObject& obj) const;
    void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap);

    void Update() override;

    void Enable();
    void Disable();

    std::vector<AudioUnit*> GetAudios() const { return mAudiosVector; }
    AudioUnit* FindAudio(const std::string& audioName);

    void AddNewAudio(FMOD::Studio::EventInstance* event);
    void AddNewAudioByName(const char* name);
    void RemoveAudio(AudioUnit* audio);
protected:
    void Reset() override {};
private:
    std::vector<AudioUnit*> mAudiosVector;
};
