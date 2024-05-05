#include "ModuleAudio.h"
#include "Globals.h"
#include "AudioSourceComponent.h"
#include "fmod_errors.h"
#include "fmod_studio.hpp"
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
	CheckError(mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));

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

std::vector<const char*> ModuleAudio::GetEventsNames()
{
	std::vector<const char*> result;
	FMOD::Studio::Bank* bankArray[EVENT_BANK_UPPERLIMIT];
	int bankCount = 0;

	mSystem->getBankList(bankArray, EVENT_BANK_UPPERLIMIT, &bankCount);
	result.reserve(bankCount);
	for (auto i = 0; i < bankCount; i++)
	{
		FMOD::Studio::Bank* bank = bankArray[i];
		FMOD::Studio::EventDescription* eventArray[EVENT_BANK_UPPERLIMIT];
		int eventDescriptionCount = 0;

		CheckError(bank->getEventList(eventArray, EVENT_BANK_UPPERLIMIT, &eventDescriptionCount));

		for (int j = 0; j < eventDescriptionCount; ++j)
		{

			// Name to string
			const int bufferSize = 256;

			char pathBuffer[bufferSize];
			int retrievedSize = 0;
			CheckError(eventArray[j]->getPath(pathBuffer, bufferSize, &retrievedSize));

			char* copyBuffer = new char[bufferSize];
			strcpy_s(copyBuffer, bufferSize, pathBuffer);

			result.push_back(copyBuffer);
		}
	}

	return result;
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

void ModuleAudio::CheckFmodErrorFunction(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		LOG("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();

	mAudiosSourceList.clear();
    return true;
}
