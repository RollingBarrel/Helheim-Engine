#include "Globals.h"
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

    AddToSearchPath(".");
    AddToSearchPath(ASSETS_PATH);
    
    CreateDirectory(ASSETS_PATH);

    CreateDirectory(LIBRARY_PATH);
    CreateDirectory(LIBRARY_MESH_PATH);
    CreateDirectory(LIBRARY_TEXTURE_PATH);
    CreateDirectory(LIBRARY_MATERIAL_PATH);
    CreateDirectory(LIBRARY_SHADER_PATH);
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
    PHYSFS_deinit();
}

// Called before render is available
bool ModuleFileSystem::Init()
{
    //Importer::CreateBinaryFile();

    //Importer::Import("Assets/Models/Triangle/Triangle.gltf");
    //Importer::Import("Shaders/basic.vs");

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

unsigned int ModuleFileSystem::Load(const char* filePath, char** buffer) const
{
    unsigned int readBytesSize = 0;
    if (Exists(filePath))
    {
        PHYSFS_File* newFile = PHYSFS_openRead(filePath);
        if (newFile != nullptr)
        {
            int fileSize = PHYSFS_fileLength(newFile);
            if (fileSize > 0)
            {
                *buffer = new char[(fileSize+1)];
                readBytesSize = PHYSFS_readBytes(newFile, *buffer, fileSize);
                if (readBytesSize != fileSize)
                {
                    LOG("Error reading from file %s: %s\n", filePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
                    delete[](*buffer);
                    *buffer = nullptr;
                }
                else
                {
                    (*buffer)[fileSize] = '\0';
                }
            }
            if (PHYSFS_close(newFile) == 0)
            {
                LOG("Error closing file %s: %s\n", filePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
            }
        }
    }
    return readBytesSize;
}

unsigned int ModuleFileSystem::Save(const char* filePath, const void* buffer, unsigned int size, bool append) const
{
    unsigned int writeBytesSize = 0;

    bool overwrite = !Exists(filePath);
    PHYSFS_File* newFile = (append) ? PHYSFS_openAppend(filePath) : PHYSFS_openWrite(filePath);

    if (newFile != nullptr)
    {
        writeBytesSize = PHYSFS_writeBytes(newFile, buffer, size);
        if (writeBytesSize != size)
        {
            LOG("Error writing to file %s: %s", filePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        }
        else
        {
            if (append)
            {
                LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), filePath);
            }
            else if (overwrite)
            {
                LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), filePath);
            }
            else
            {
                LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), filePath);
            }
        }

        if (PHYSFS_close(newFile) == 0)
        {
            LOG("Error closing file %s: %s\n", filePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        }
    }
    return writeBytesSize;
}

bool ModuleFileSystem::Copy(const char* sourceFilePath, const char* destinationFilePath)
{
    char* readBuffer = nullptr;
    int readBufferSize = Load(sourceFilePath, &readBuffer);

    if (readBufferSize > 0)
    {
        int writeBufferSize = Save(destinationFilePath, readBuffer, readBufferSize);

        if (writeBufferSize == 0)
        {
            LOG("Error unable to copy into new directory %s", destinationFilePath);
            return false;
        }
    }
    else
    {
        return false;
    }
   
    return true;
}

bool ModuleFileSystem::CreateDirectory(const char* directory)
{ 
    if (IsDirectory(directory) == false)
    {
        if (PHYSFS_mkdir(directory) != 0)
        {
            LOG("Succes creating file/dir stat: %s", directory);
            return true;
        }
        else
        {
            LOG("Error creating file/dir stat: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));            
            return false;
        }
    }  
}

bool ModuleFileSystem::Exists(const char* filePath) const
{
    return PHYSFS_exists(filePath);
}

bool ModuleFileSystem::IsDirectory(const char* directoryPath) const
{
    PHYSFS_Stat stat;
    if (!PHYSFS_stat(directoryPath, &stat))
        LOG("Error obtaining file/dir stat: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    return(stat.filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY);
}

bool ModuleFileSystem::AddToSearchPath(const char* path)
{
    if (!PHYSFS_mount(path, nullptr, 1))
    {
        LOG("Error while setting path (%s): %s\n", path, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        return false;
    }
    else
        return true;
}

void ModuleFileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& files, std::vector<std::string>& directories)
{
    if (Exists(directory))
    {
        char** fileList = PHYSFS_enumerateFiles(directory);

        std::string path = directory + std::string("/");

        for (auto file = fileList; *file != nullptr; ++file)
        {
            path += *file;

            if (IsDirectory(path.c_str()))
            {
                //TODO PathNode 
                directories.push_back(path);
                DiscoverFiles(path.c_str(), files, directories);
                path = directory + std::string("/");
            }
            else
            {
                //TODO Assets To Display, except .bin

                //TODO Read Meta and create Library Link
                files.push_back(path);
                path = directory + std::string("/");
            }
        }


        PHYSFS_freeList(fileList);
    }
}


