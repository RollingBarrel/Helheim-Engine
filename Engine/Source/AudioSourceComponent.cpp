#include "AudioSourceComponent.h"
#include "FmodUtils.h"

#include "GameObject.h"
#include "Application.h"


AudioSourceComponent::AudioSourceComponent(GameObject* ownerGameObject): Component(ownerGameObject,ComponentType::AUDIOSOURCE)
{
}

AudioSourceComponent::~AudioSourceComponent()
{
	Reset();
}

void AudioSourceComponent::SetEventInstance(FMOD::Studio::EventInstance* event)
{
	mEventInstance = event;

	if (mEventInstance != nullptr) {

		// Update parameters
		event->getDescription(&mEventDescription);
		UpdateParameters();
	}
}

void AudioSourceComponent::SetEventByName(const char* eventName)
{
	SetEventInstance(FmodUtils::GetEventByName(eventName));
	size_t eventNameLength = strlen(eventName);

	mName = eventName;
}

void AudioSourceComponent::UpdateParameterValue(const char* name, float value)
{
	mEventInstance->setParameterByName(name, value);

	auto it = mParameters.find(name);
	if (it != mParameters.end()) {
		it->second = value;
	}

}

void AudioSourceComponent::Update()
{
	// UPDATE 3D parameters
	float3 gameobjectPosition = GetOwner()->GetPosition();

	FMOD_3D_ATTRIBUTES attributes = { { 0 } };

	attributes.position.x = gameobjectPosition.x;
	attributes.position.z = gameobjectPosition.z;

	attributes.forward.z = 1.0f;
	attributes.up.y = 1.0f;

	// Play Audio
	mEventInstance->set3DAttributes(&attributes);
}

void AudioSourceComponent::Play()
{
	mEventInstance->start();
}

void AudioSourceComponent::PlayOneShot()
{
	FMOD::Studio::EventInstance* eventInstance = NULL;
	mEventDescription->createInstance(&eventInstance);

	eventInstance->start();
	eventInstance->release();
}

void AudioSourceComponent::Stop()
{
	mEventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
}

Component* AudioSourceComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void AudioSourceComponent::Save(Archive& archive) const
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));

	archive.AddString("EventName", mName.c_str());

	std::vector<const char*> parameterKeys;
	std::vector<float> parameterValues;

	for (const auto& pair : mParameters) {
		parameterKeys.push_back(pair.first);
		parameterValues.push_back(pair.second);
	}

	std::vector<Archive> objectArray;
	for (auto i = 0; i < parameterKeys.size(); i++)
	{
		Archive dataArchive;
		dataArchive.AddString("ParametersKey", parameterKeys[i]);
		dataArchive.AddFloat("ParametersValue", parameterValues[i]);

		objectArray.push_back(dataArchive);
	}

	archive.AddObjectArray("ParametersVariables", objectArray);
}

void AudioSourceComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	if (data.HasMember("EventName") && data["EventName"].IsString()) {
		SetEventByName(data["EventName"].GetString());
	}

	if (data.HasMember("ParametersVariables") && data["ParametersVariables"].IsArray()) {
		UpdateParameters();

		const auto& array = data["ParametersVariables"].GetArray();

		for (unsigned int i = 0; i < array.Size(); ++i)
		{
			LOG("Element %d: Has ParametersKey: %s\n     Has ParametersValue: %s\n",
				i, array[i]["ParametersKey"].IsString() ? "true" : "false", array[i]["ParametersValue"].IsFloat() ? "true" : "false");
			const char* value;
			//if (array[i].HasMember("ParametersKey") && array[i]["ParametersKey"].IsString()
			//	&& array[i].HasMember("ParametersValue") && array[i]["ParametersValue"].IsFloat()) {
				value = array[i]["ParametersKey"].GetString();
				float key = array[i]["ParametersValue"].GetFloat();

				UpdateParameterValue(value, key);
			//}
		}

		
	}
}

void AudioSourceComponent::Enable()
{
	Play();
}

void AudioSourceComponent::Disable()
{
	Stop();
}

void AudioSourceComponent::Reset()
{
	if (mEventInstance != nullptr) {
		delete mEventInstance;
		mEventInstance = nullptr;
	}

	if (mEventDescription != nullptr) {
		delete mEventDescription;
		mEventDescription = nullptr;
	}

	mParameters.clear();
}

void AudioSourceComponent::UpdateParameters()
{
	mParameters.clear();
	int count = 0;
	mEventDescription->getParameterDescriptionCount(&count);

	for (auto i = 0; i < count; i++)
	{
		FMOD_STUDIO_PARAMETER_DESCRIPTION parameter;
		mEventDescription->getParameterDescriptionByIndex(i, &parameter);

		mParameters.insert(std::make_pair(parameter.name, parameter.defaultvalue));
	}

}
