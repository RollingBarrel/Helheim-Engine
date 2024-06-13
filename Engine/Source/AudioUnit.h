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
    FMOD::Studio::EventDescription* GetDescription() const { return mEventDescription; };
    int GetPreviewID() const { return mPreviewPlayID; };

    void GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& value);

    // Set event
    void SetEventInstance(FMOD::Studio::EventInstance* event);
    void SetEventByName(const char* eventName);
    void GetParametersMaxMin(const char* eventName, float& max, float& min);

    // Update parameters
    void UpdateParameterValueByName(const char* name, const float value);

    void Play();
    void Pause(bool fadeout);
    void Release();
private:

    std::string mName = "";

    FMOD::Studio::EventDescription* mEventDescription = nullptr;

    int mPreviewPlayID = -1;
};

