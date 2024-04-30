#pragma once
#include "Module.h"

#include <map>

#define CheckError(result) FmodUtils::CheckFmodError(result)
#define EVENT_BANK_UPPERLIMIT 64

namespace FMOD {
	namespace Studio {
		class System; 
		class Bank;
		class EventInstance;
	}
	class System; 
}

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

	FMOD::Studio::System* GetFMODSystem() {return mSystem;};
	void PauseAllChannels();

	bool CleanUp();

private:
	FMOD::Studio::System* mSystem = nullptr;
	FMOD::System* mCoreSystem = nullptr;

	FMOD::Studio::Bank* mStringBank = nullptr;
	FMOD::Studio::Bank* mMasterBank = nullptr;
	FMOD::Studio::Bank* mSFXBank = nullptr;
	FMOD::Studio::Bank* mMusicBank = nullptr;

	bool mPaused = false;

};

