#include "AudioUnit.h"
#include "ModuleAudio.h"

#include "GameObject.h"
#include "Application.h"

#include "fmod_studio.hpp"

#define CheckError(result) ModuleAudio::CheckFmodError(result)

AudioUnit::AudioUnit()
{
}

AudioUnit::~AudioUnit()
{

}


void AudioUnit::GetParametersNameAndValue(std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& values)
{
	if (mPreviewPlayID != -1) 
	{
		App->GetAudio()->GetParameters(mEventDescription, mPreviewPlayID, index, names, values);
	}
}

void AudioUnit::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	if (event != nullptr)
	{
		// Update parameters
		event->getDescription(&mEventDescription);
	}
}

void AudioUnit::SetEventByName(const char* eventName)
{
	FMOD::Studio::System* system = App->GetAudio()->GetFMODSystem();
	FMOD::Studio::EventDescription* eventDescription = nullptr;
	system->getEvent(eventName, &eventDescription);

	FMOD::Studio::EventInstance* event = nullptr;
	eventDescription->createInstance(&event);

	SetEventInstance(event);
	size_t eventNameLength = strlen(eventName);

	mName = eventName;
}

void AudioUnit::GetParametersMaxMin(const char* eventName, float& max, float& min)
{
	FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	CheckError(mEventDescription->getParameterDescriptionByName(eventName, &paramDesc));

	max = paramDesc.maximum;
	min = paramDesc.minimum;
}

void AudioUnit::UpdateParameterValueByName(const char* name, const float value)
{
	if (mPreviewPlayID != -1) 
	{
		App->GetAudio()->UpdateParameter(mEventDescription, mPreviewPlayID, name, value);
	}
}

void AudioUnit::Save(JsonObject& obj) const
{
	obj.AddString("Name", mName.c_str());
	obj.AddInt("PreviewPlayID", mPreviewPlayID);
}

void AudioUnit::Load(const JsonObject& data)
{
	mName = data.GetString("Name");
	mPreviewPlayID = data.GetInt("PreviewPlayID");

	SetEventByName(mName.c_str());
}

void AudioUnit::Play()
{
	if (mPreviewPlayID == -1)
	{
		// New audio
		mPreviewPlayID = App->GetAudio()->Play(mEventDescription);
	}
	else 
	{
		// Continue
		App->GetAudio()->Play(mEventDescription, mPreviewPlayID);
	}
}

void AudioUnit::Pause(bool fadeout)
{
	if (mPreviewPlayID != -1)
	{
		App->GetAudio()->Pause(mEventDescription, mPreviewPlayID, fadeout);
	}
}

void AudioUnit::Release()
{
	if (mPreviewPlayID != -1)
	{
		Pause(true);
		App->GetAudio()->Release(mEventDescription, mPreviewPlayID);
		mPreviewPlayID = -1;
	}
}


