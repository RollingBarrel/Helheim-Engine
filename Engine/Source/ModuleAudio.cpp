#include "ModuleAudio.h"
#include "Globals.h"
#include "AudioSourceComponent.h"

#include "fmod_studio.hpp"
#include "FmodUtils.h"
#include "Application.h"

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	// Instantiate Fmod studio
	CheckError( FMOD::Studio::System::create(&mSystem) ); // Create the Studio System object.

//#if DEBUG
	CheckError( mSystem->initialize(1024, FMOD_STUDIO_INIT_MEMORY_TRACKING, FMOD_INIT_MEMORY_TRACKING, 0));
	//#elif RELEASE
		//CheckError(mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
//#endif
	CheckError( mSystem->getCoreSystem(&mCoreSystem));

	// Load bank
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mStringBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMasterBank) );
	CheckError(mSystem->loadBankFile(("Assets/FMOD/SFX.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mSFXBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Music.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMusicBank) );

	return true;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudio::Update(float dt)
{
	mSystem->update();

	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleAudio::AudioPause()
{
	for (auto audioSource : mAudiosSourceList) 
	{
		audioSource->PauseCurrentInstance();
	}
}

void ModuleAudio::AudioResume()
{
	for (auto audioSource : mAudiosSourceList) 
	{
		audioSource->ResumeCurrentInstance();
	}
}

void ModuleAudio::EngineStop()
{
	for (auto audioSource : mAudiosSourceList) 
	{
		audioSource->CleanCurrentInstance();
	}

	mAudiosSourceList = std::vector<AudioSourceComponent*>();
}

void ModuleAudio::AddToAudiosList(AudioSourceComponent* audioSource)
{
	mAudiosSourceList.push_back(audioSource);
}

int ModuleAudio::GetMemoryUsage()
{
	int currentAllocated, maxAllocated;
	FMOD_RESULT result = FMOD_Memory_GetStats(&currentAllocated, &maxAllocated, 1);
	// Just get fmod memory
	return currentAllocated;
}

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();

	mAudiosSourceList.clear();
    return true;
}
