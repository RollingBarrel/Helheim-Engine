#pragma once
#include <Script.h>
#include <string>
#include <unordered_map>
#include "Macros.h"
#include "float3.h"

namespace FMOD {
	namespace Studio {
		class EventInstance;
		class EventDescription;
	}
}

class AudioSourceComponentNew;
class AudioUnit;

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

	int Play(std::string audioName, float3 audioPosition, std::unordered_map<std::string, int> parameters = {});
	void PlayOneShot(std::string audioName, float3 audioPosition, std::unordered_map<std::string, int> parameters = {});

	static AudioManager* mInstance;
	 
	AudioSourceComponentNew* mAudioSources = nullptr;

	std::vector<AudioUnit*> mBackgroundAudioList;
	std::vector<AudioUnit*> mSFXList;

	int mBackgroundAudioCounter = 0;

};

