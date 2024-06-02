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

class ENGINE_API AudioSourceComponentNew : public Component
{
public:
    AudioSourceComponentNew(GameObject* ownerGameObject);
    AudioSourceComponentNew(const AudioSourceComponentNew& original, GameObject* owner);
    ~AudioSourceComponentNew();

    Component* Clone(GameObject* owner) const;
    void Save(Archive& archive) const;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);
    void Update() override;

    void Enable();
    void Disable();

    std::vector<AudioUnit*> GetAudios() const { return mAudiosVector; }

    void AddNewAudio(FMOD::Studio::EventInstance* event);
    void AddNewAudioByName(const char* name);
    void RemoveAudio(AudioUnit* audio);
protected:
    void Reset() override {};
private:
    std::vector<AudioUnit*> mAudiosVector;
};
