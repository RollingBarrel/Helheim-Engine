#pragma once
#include "Component.h"

#include <vector>
#include <map>

namespace FMOD {
    namespace Studio {
        class EventInstance; 
        class EventDescription; 
    }
}

class ENGINE_API AudioSourceComponent :
    public Component
{
public:
    AudioSourceComponent(GameObject* ownerGameObject);
    AudioSourceComponent(const AudioSourceComponent& original, GameObject* owner);
    ~AudioSourceComponent();

    std::string GetName() const{ return mName; };
    std::map<int, float> GetParameters() const { return mParameters; };
    void GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& value);
    FMOD::Studio::EventDescription* GetEventDescription() {return mEventDescription;};

    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);

    void UpdateParameterValueByIndex(int index, float value);
    
    void Update() override;
    void Play();
    void PlayOneShot();
    void Stop();
    Component* Clone(GameObject* owner) const;
    void Save(Archive& archive) const;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);

    void Enable();
    void Disable();

protected:
    void Reset();
private:
    std::string mName = "";

    FMOD::Studio::EventInstance* mEventInstance = nullptr; 
    FMOD::Studio::EventDescription* mEventDescription = nullptr;
    std::map<int, float> mParameters;

    void UpdateParameters();
    

};

