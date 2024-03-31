#pragma once
#include "Component.h"

#include "fmod_studio.hpp"
#include <vector>
#include <map>

class AudioSourceComponent :
    public Component
{
public:
    AudioSourceComponent(GameObject* ownerGameObject);
    ~AudioSourceComponent();

    std::string GetName() { return mName; };
    std::map<const char*, float> GetParameters() { return mParameters; };
    FMOD::Studio::EventDescription* GetEventDescription() {return mEventDescription;};

    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);

    void UpdateParameterValue(const char* name, float value);
    
    void Update();
    Component* Clone(GameObject* owner) const;
    void Save(Archive& archive) const;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner);

    void Enable();
    void Disable();

protected:
    void Reset();
private:
    std::string mName = "";

    FMOD::Studio::EventInstance* mCurrentInstance = nullptr; 
    FMOD::Studio::EventDescription* mEventDescription = nullptr;
    std::map<const char*, float> mParameters;

    void UpdateParameters();
    

};

