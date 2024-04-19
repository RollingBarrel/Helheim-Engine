#pragma once
#include "Module.h"

#include "fmod_studio.hpp"
#include <map>

#define CheckError(result) FmodUtils::CheckFmodError(result)
#define EVENT_BANK_UPPERLIMIT 64

class ModuleAudio :
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

	bool CleanUp();

private:
	FMOD::Studio::System* mSystem = nullptr;
	FMOD::System* mCoreSystem = nullptr;

	FMOD::Studio::Bank* mStringBank = nullptr;
	FMOD::Studio::Bank* mMasterBank = nullptr;
	FMOD::Studio::Bank* mSFXBank = nullptr;

	FMOD::Studio::EventInstance* mCurrentInstance = nullptr;


};

