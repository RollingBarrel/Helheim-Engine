#include "ModuleAudio.h"
#include "Globals.h"

#include "fmod.hpp"
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
	CheckError( mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
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
	if (!mPaused) 
	{
		mSystem->update();
		if (!mActiveAudiosList.empty())
		{
			FMOD_STUDIO_PLAYBACK_STATE state;
			mActiveAudiosList[0]->getPlaybackState(&state);
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
			{
				mActiveAudiosList[0]->stop(FMOD_STUDIO_STOP_IMMEDIATE);
			}
			LOG("a");
		}
	}
	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleAudio::PauseAllChannels()
{
	mPaused = true;
	//FMOD::System* coreSystem = nullptr;
	//mSystem->getCoreSystem(&coreSystem);

	//int numChannels = 0;
	//coreSystem->getChannelsPlaying(&numChannels);
	//for (int i = 0; i < numChannels; ++i) {
	//	FMOD::Channel* channel = nullptr;
	//	coreSystem->getChannel(i, &channel);

	//	channel->setPaused(true);

	//	bool playstete;
	//	channel->getPaused(&playstete);

	//	LOG("%d\n", playstete);
	//}
	//FMOD::Studio::Bank* masterBank;
	//mSystem->getBank("Master Bank", &masterBank);
	//masterBank->unload(); // Unload the master bank
}

void ModuleAudio::AddToActiveAudiosList(FMOD::Studio::EventInstance* eventInstance)
{
	mActiveAudiosList.push_back(eventInstance);
}

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();
    return true;
}
