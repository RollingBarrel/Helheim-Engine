#include "ModuleAudio.h"
#include "Globals.h"

#include "fmod.hpp"
#include "FmodUtils.h"


ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	FMOD_RESULT result = FMOD_OK;

	// Instantiate Fmod studio
	CheckError( FMOD::Studio::System::create(&mSystem) ); // Create the Studio System object.
	CheckError( mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0));
	CheckError( mSystem->getCoreSystem(&mCoreSystem));

	// Load bank
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.strings.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mStringBank) );
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Master.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mMasterBank) );
	CheckError(mSystem->loadBankFile(("Assets/FMOD/SFX.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mSFXBank));
	CheckError( mSystem->loadBankFile(("Assets/FMOD/Vehicles.bank"), FMOD_STUDIO_LOAD_BANK_NORMAL, &mSFXBank) );

	//FMOD::Studio::EventDescription* eventArray[EVENT_BANK_UPPERLIMIT];
	//int eventDescriptionCount = 0;

	//CheckError( mSFXBank->getEventList(eventArray, EVENT_BANK_UPPERLIMIT, &eventDescriptionCount));

	//for (int i = 0; i < eventDescriptionCount; ++i) {
	//	const int bufferSize = 256; 

	//	char pathBuffer[bufferSize]; 
	//	int retrievedSize = 0; 
	//	CheckError( eventArray[i]->getPath(pathBuffer, bufferSize, &retrievedSize ));
	//}

	//FMOD::Studio::EventDescription* eventDescription = nullptr;
	//CheckError( mSystem->getEvent("event:/Ambience/Country", &eventDescription));

	//FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
	//CheckError( eventDescription->getParameterDescriptionByName("Surface", &paramDesc));

	//CheckError( eventDescription->createInstance(&currentInstance));
	//CheckError( currentInstance->start());
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

bool ModuleAudio::CleanUp()
{
	mSystem->unloadAll();
	mSystem->release();
    return true;
}
