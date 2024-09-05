#pragma once
#include "Module.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include "float3.h"

#define EVENT_BANK_UPPERLIMIT 64
#define CheckError(result) ModuleAudio::CheckFmodError(result)
#define CheckFmodError(_result) ModuleAudio::CheckFmodErrorFunction(_result, __FILE__, __LINE__)

namespace FMOD {
	namespace Studio {
		class System; 
		class Bank;
		class EventInstance;
		class EventDescription;
	}
	class System;
	class Sound;
	class Channel;
	class ChannelGroup;
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
	// Start
	int Play(const FMOD::Studio::EventDescription* eventDescription, const int id = -1);
	void Pause(const FMOD::Studio::EventDescription* eventDescription, const int id, bool pause);

	int PlayOneShot(const std::string& fileName, float3 eventPosition,const int id = -1);

	// Kill instance
	void Stop(const FMOD::Studio::EventDescription* eventDescription, const int id);
	void Release(const FMOD::Studio::EventDescription* eventDescription, const int id);

	
	// Update
	void GetParameters(const FMOD::Studio::EventDescription* eventDescription, const int id, std::vector<int>& index, std::vector<const char*>& names, std::vector<float>& values);
	void UpdateParameter(const FMOD::Studio::EventDescription* eventDescription, const int id ,const std::string& parameterName, const float parameterValue);
	void SetEventPosition(const FMOD::Studio::EventDescription* eventDescription, const int id , float3 eventPosition);

	int GetMemoryUsage() const;
	void GetInstances(std::map<std::string, int>& instances) const;

	float GetVolume(std::string busname) const;
	void SetVolume(std::string busname, float value) const;

	static void CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line);

private:
	FMOD::Studio::EventInstance* FindEventInstance(const FMOD::Studio::EventDescription* eventDescription, const int id);
	void AddIntoEventList(const FMOD::Studio::EventDescription* eventDescription);

	FMOD::Studio::System* mSystem = nullptr;
	FMOD::System* mCoreSystem = nullptr;

	FMOD::Studio::Bank* mStringBank = nullptr;
	FMOD::Studio::Bank* mMasterBank = nullptr;
	FMOD::Studio::Bank* mSFXBank = nullptr;
	FMOD::Studio::Bank* mMusicBank = nullptr;
	FMOD::Studio::Bank* mAmbBank = nullptr;
	FMOD::Studio::Bank* mUiBank = nullptr;

	bool mPaused = false;
	bool mStopped = false;

	//std::unordered_map<int, FMOD::Sound*> mSounds;  // Map of sounds by ID
	FMOD::ChannelGroup* mChannelGroup = nullptr;

	std::vector<FMOD::Studio::EventDescription*> mActiveEvent;

};