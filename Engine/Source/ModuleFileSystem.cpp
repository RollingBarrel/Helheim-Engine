#include "ModuleFileSystem.h"
#include "Importer.h"

ModuleFileSystem::ModuleFileSystem()
{

}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{

}

// Called before render is available
bool ModuleFileSystem::Init()
{
    Importer importer;
    importer.CreateBinaryFile();
    return true;
}

// Called every draw update
update_status ModuleFileSystem::Update()
{
    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
    return true;
}

