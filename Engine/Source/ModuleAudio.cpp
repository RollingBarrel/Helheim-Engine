#include "ModuleAudio.h"
#include "Globals.h"

#include "AK/SoundEngine/Common/AkMemoryMgr.h"                 // Memory Manager interface
#include "AK/SoundEngine/Common/AkModule.h"                    // Default memory manager

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);

    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        return false;
    }
	return true;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudio::Update(float dt)
{

	return UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{

    AK::MemoryMgr::Term();

    return true;
}
