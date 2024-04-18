#pragma once
#include "Component.h"

#include "fmod_studio.hpp"
#include <vector>
#include <map>

class ENGINE_API AudioSourceComponent :
    public Component
{
public:
    AudioSourceComponent(GameObject* ownerGameObject);
    ~AudioSourceComponent();

    // Must return by reference
    std::string GetName() { return mName; };
    std::map<FMOD_STUDIO_PARAMETER_ID, float> GetParameters() { return mParameters; };
    void GetParametersIDsAndValues(std::vector<unsigned int>& data1, std::vector<unsigned int>& data2, std::vector<const char*>& names, std::vector<float>& value);
    FMOD::Studio::EventDescription* GetEventDescription() {return mEventDescription;};

    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);

    void UpdateParameterValue(const char* name, float value);
    void UpdateParameterValueByIds(unsigned data1, unsigned data2, float value);
    
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
    std::map<FMOD_STUDIO_PARAMETER_ID, float> mParameters;

    void UpdateParameters();
    

};

