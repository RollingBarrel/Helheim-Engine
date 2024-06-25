#include "ImporterStateMachine.h"
#include "ResourceStateMachine.h"
#include "AnimationStateMachine.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
#include "ModuleFileSystem.h"
#include <cassert>

ResourceStateMachine* Importer::StateMachine::Import(const char* assetsPath, unsigned int uid)
{
    // Copy assets path into correct library path
    AnimationStateMachine* newSM = new AnimationStateMachine();
    newSM->LoadResource(assetsPath);
    ResourceStateMachine* newResource = new ResourceStateMachine(uid, newSM);

	assert(newSM->GetNumStates() > 0);

	const char* libraryFile = EngineApp->GetFileSystem()->GetLibraryFile(uid);


	char* fileBuffer = nullptr;
	int size = App->GetFileSystem()->Load(assetsPath, &fileBuffer);
	std::string path = App->GetFileSystem()->GetLibraryFile(uid, true);
	App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);
	

    EngineApp->GetEngineResource()->CreateAssetsMeta(*newResource, assetsPath);

    return newResource;
}
