#pragma once
#include "Component.h"
#include "float3.h"

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

    // Set event
    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);
    void GetParametersMaxMin(const char* eventName, float& max, float& min);
    void SetPositionWithGameObject(bool with) { mPositionWithGameObject = with; };

    // Update parameters
    void UpdateParameterValueByIndex(int index, float value);
    void UpdateParameterValueByName(const char* name, float value);
    void SmoothUpdateParameterValueByName(const char* name, float targetValue, float transitionTime);
    void UpdatePosition(float3 position);

    // Engine state control
    void PauseCurrentInstance();
    void ResumeCurrentInstance();
    void CleanCurrentInstance();
    
    void Update() override;
    void Play();
    void PlayWithVolume(float volume);
    void PlayOneShot();
    void PlayOneShotPosition(float3 position);
    void Stop(bool fadeout);
    Component* Clone(GameObject* owner) const;
    void Save(Archive& archive) const;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);

    void Enable();
    void Disable();

protected:
    void Reset();
private:
    float GetParameterValueByIndex(int index);
    void UpdateParameters();

    std::string mName = "";

    FMOD::Studio::EventInstance* mEventInstance = nullptr; 
    FMOD::Studio::EventDescription* mEventDescription = nullptr;
    std::map<int, float> mParameters;
    std::map<std::string, int> mNameToParameters;

    bool mPositionWithGameObject = true;
};

