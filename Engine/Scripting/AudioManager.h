#pragma once
#include <Script.h>
#include <string>
#include <map>
#include "Macros.h"
#include "float3.h"

namespace FMOD {
	namespace Studio {
		class EventInstance;
		class EventDescription;
	}
}

class AudioSourceComponentNew;

GENERATE_BODY(AudioManager);
class AudioManager : public Script
{
	FRIEND(AudioManager)

public:
	static AudioManager* GetInstance();

	void Start();
	void Update();

private:
	AudioManager(GameObject* owner);
	~AudioManager();

	int Play(std::string audioName, float3 audioPosition);
	void PlayOneShot(std::string audioName, float3 audioPosition);

	static AudioManager* mInstance;

	AudioSourceComponentNew* mAudioSources = nullptr;

	std::vector<FMOD::Studio::EventInstance*> mAudiosSourceList;

};

