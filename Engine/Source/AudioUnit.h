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

class ENGINE_API AudioUnit
{
public:
    AudioUnit();
    ~AudioUnit();

    std::string GetName() const { return mName; };
    std::map<int, float> GetParameters() const { return mParameters; };
    FMOD::Studio::EventDescription* GetDescription() const { return mEventDescription; };
    int GetPreviewID() const { return mPreviewPlayID; };

    void GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& value);

    // Set event
    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);
    void GetParametersMaxMin(const char* eventName, float& max, float& min);
    void SetPositionWithGameObject(bool with) { mPositionWithGameObject = with; };

    // Update parameters
    void UpdateParameterValueByIndex(int index, float value);
    void UpdateParameterValueByName(const char* name, const float value);
    void SmoothUpdateParameterValueByName(const char* name, float targetValue, float transitionTime);
    void UpdatePosition(float3 position);

    // Engine state control
    void PauseCurrentInstance();
    void ResumeCurrentInstance();
    void CleanCurrentInstance();

    FMOD::Studio::EventInstance* CreateEventInstance();

    void Play();
    void PlayWithVolume(float volume);
    void PlayOneShotPosition(float3 position);
    void Stop(bool fadeout);

protected:
    void Reset();
private:
    float GetParameterValueByIndex(int index);
    void UpdateParameters();

    std::string mName = "";

    FMOD::Studio::EventInstance* mEventInstance = nullptr;
    FMOD::Studio::EventDescription* mEventDescription = nullptr;
    // Index - value
    std::map<int, float> mParameters;
    
    // Name - value
    std::map<std::string, int> mNameToParameters;

    bool mPositionWithGameObject = true;

    int mPreviewPlayID = -1;
};

