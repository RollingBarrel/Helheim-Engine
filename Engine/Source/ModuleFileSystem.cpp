#include "ModuleFileSystem.h"
#include "Importer.h"
#include "ImporterTexture.h"

#include "physfs.h"

ModuleFileSystem::ModuleFileSystem()
{
    PHYSFS_init(nullptr);

    if (PHYSFS_setWriteDir(".") == 0)
    {
        LOG("File System error while creating write dir: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    if (!PHYSFS_mount(".", nullptr, 1))
    {
        LOG("Error while setting path (%s): %s\n",".", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    CreateDirectory(LIBRARY_PATH);
    CreateDirectory(LIBRARY_MESH_PATH);
    CreateDirectory(LIBRARY_TEXTURE_PATH);
    CreateDirectory(LIBRARY_MATERIAL_PATH);
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
    PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init()
{
    Importer::CreateBinaryFile();

    //TODO CREATE LIBRARY FILE SYSTEM FOLDERS

    //CreateDirectoryLibrary();
    

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

bool ModuleFileSystem::CreateDirectory(const char* directory)
{
    bool ret = true;
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(directory, &stat))
        LOG("Error obtaining file/dir stat: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    ret = (stat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY);
  
    if (ret == false)
    {
        if (PHYSFS_mkdir(directory) != 0)
        {
            LOG("Succes creating file/dir stat: %s", directory);
            ret = true;
        }
        else
        {
            LOG("Error creating file/dir stat: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));            
        }
    }  

    return ret;
}

