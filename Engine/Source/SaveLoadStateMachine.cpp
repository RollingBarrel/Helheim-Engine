#include "SaveLoadStateMachine.h"
#include "ResourceStateMachine.h"
#include "AnimationStateMachine.h"
#include "Application.h"
#include "ModuleFileSystem.h"

ENGINE_API void Importer::StateMachine::Save(const ResourceStateMachine* ourStateMachine)
{
	const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourStateMachine->GetUID(), true);
	ourStateMachine->GetStateMachine()->SaveResource(libraryPath);
}

ENGINE_API ResourceStateMachine* Importer::StateMachine::Load(const char* fileName, unsigned int uid)
{
	AnimationStateMachine* newSm = new AnimationStateMachine();

	newSm->LoadResource(fileName); //Check the full path in debug

	return new ResourceStateMachine(uid, newSm);
}
