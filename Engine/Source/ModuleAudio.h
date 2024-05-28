#pragma once
#include "Module.h"

#include <vector>
#include <string>

#define EVENT_BANK_UPPERLIMIT 64
#define CheckError(result) ModuleAudio::CheckFmodError(result)
#define CheckFmodError(_result) ModuleAudio::CheckFmodErrorFunction(_result, __FILE__, __LINE__)

namespace FMOD {
	namespace Studio {
		class System; 
		class Bank;
		class EventInstance;
	}
	class System;
}
enum FMOD_RESULT : int;

class AudioSourceComponent;

class ENGINE_API ModuleAudio :
    public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init();
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

	FMOD::Studio::System* GetFMODSystem() {return mSystem;};
	std::vector<const char*> GetEventsNames();

	void AudioPause();
	void AudioResume();

	void EngineStop();

	void AddToAudiosList(AudioSourceComponent* audioSource);

	int GetMemoryUsage() const;
	float GetVolume(std::string busname) const;
	void SetVolume(std::string busname, float value) const;

	static void CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line);

private:
	FMOD::Studio::System* mSystem = nullptr;
	FMOD::System* mCoreSystem = nullptr;

	FMOD::Studio::Bank* mStringBank = nullptr;
	FMOD::Studio::Bank* mMasterBank = nullptr;
	FMOD::Studio::Bank* mSFXBank = nullptr;
	FMOD::Studio::Bank* mMusicBank = nullptr;
	FMOD::Studio::Bank* mVehicleBank = nullptr;

	bool mPaused = false;
	bool mStopped = false;

	std::vector<AudioSourceComponent*> mAudiosSourceList;
};

