#define _CRT_SECURE_NO_WARNINGS
#include "Application.h"
#include "Globals.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ImporterTexture.h"

#include "ProjectPanel.h"

#include <stdio.h>

#include "physfs.h"

ModuleFileSystem::ModuleFileSystem() 
{
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
    PHYSFS_deinit();
}

bool ModuleFileSystem::Init()
{
    PHYSFS_init(nullptr);

    if (PHYSFS_setWriteDir(".") == 0)
    {
        LOG("File System error while creating write dir: %s\n", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    }

    AddToSearchPath("../Scripting/Output");
    AddToSearchPath(".");
    //AddToSearchPath(LIBRARY_PATH);
    //AddToSearchPath(ASSETS_PATH);

    CreateDirectory(ASSETS_PATH);
    CreateDirectory(ASSETS_MODEL_PATH);
    CreateDirectory(ASSETS_TEXTURE_PATH);
    CreateDirectory(ASSETS_MATERIAL_PATH);
    CreateDirectory(ASSETS_SCENES_PATH);
    CreateDirectory(ASSETS_PREFABS_PATH);
    CreateDirectory(ASSETS_SCRIPT_PATH);
    CreateDirectory(ASSETS_NAVMESH_PATH);
    CreateDirectory(ASSETS_IBL_PATH);
    CreateDirectory(LIBRARY_PATH);

    CreateDirectory(INTERNAL_ASSETS_PATH);
    CreateDirectory(INTERNAL_ASSETS_SCENES_PATH);
    CreateDirectory(INTERNAL_ASSETS_FONTS_PATH);

    mRoot = new PathNode("Assets");
    DiscoverFiles("Assets", mRoot);
    return true;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
    delete mRoot;
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
                LOG("Added %u data to [%s%s]", size, PHYSFS_getWriteDir(), filePath);       //When loading a scene ModuleFilesystem should not save anything
            }
        }

        if (PHYSFS_close(newFile) == 0)
        {
            LOG("Error closing file %s: %s\n", filePath, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
        }
    }
    return writeBytesSize;
}

bool ModuleFileSystem::CopyRelativePath(const char* sourceFilePath, const char* destinationFilePath) const
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

bool ModuleFileSystem::CopyAbsolutePath(const char* sourceFilePath, const char* destinationFilePath) const
{
    FILE *src;
    FILE *dst;

    char buffer[4096];
    size_t n;
    errno_t err;

    if ((err = fopen_s(&src, sourceFilePath, "rb")) == 0 )
    {      
        if ((err = fopen_s(&dst, destinationFilePath, "wb")) == 0)
        {
            while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0)
            {
                fwrite(buffer, 1, n, dst);
            }

            fclose(dst);
        }  
        else
        {
            LOG("Unable to open File %s", destinationFilePath);
        }      

        fclose(src);
    }
    else
    {
        LOG("Unable to open File %s", sourceFilePath);
    }
  

    return true;
}

bool ModuleFileSystem::DeleteDirectory(const char* filePath) const
{
    return false;
}

bool ModuleFileSystem::RemoveFile(const char* filePath) const
{
    if (PHYSFS_exists(filePath) && !IsDirectory(filePath))
    {
        PHYSFS_delete(filePath);
        return true;
    }
    return false;
}

bool ModuleFileSystem::CreateDirectory(const char* directory) const
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

    return false;
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

const char* ModuleFileSystem::GetLibraryFile(unsigned int uid, bool createDir) const
{
    char* ret = new char[sizeof(LIBRARY_PATH) + 13];
    ret[0] = '\0';
    std::string sUid = std::to_string(uid);
    strcat(ret, LIBRARY_PATH);
    ret[sizeof(LIBRARY_PATH) - 1] = sUid[0];
    ret[sizeof(LIBRARY_PATH)] = sUid[1];
    ret[sizeof(LIBRARY_PATH) + 1] = '/';
    ret[sizeof(LIBRARY_PATH) + 2] = '\0';
    if (createDir && !Exists(ret))
        CreateDirectory(ret);
    strcat(ret, sUid.c_str());
    return ret;
}

int64_t ModuleFileSystem::GetLastModTime(const char* file) const
{
    PHYSFS_Stat stat;
    if (PHYSFS_stat(file, &stat) == 0)
        return 0;
    return stat.modtime;
}

int64_t ModuleFileSystem::GetCreationTime(const char* file) const
{
    PHYSFS_Stat stat;
    if (PHYSFS_stat(file, &stat) == 0)
        return 0;
    return stat.createtime;
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

const char* ModuleFileSystem::GetBaseDirectory() const
{
    return PHYSFS_getBaseDir();
}

const char* ModuleFileSystem::GetWriteDirectory() const
{
    return PHYSFS_getWriteDir();
}

void ModuleFileSystem::DiscoverFiles(const char* directory, const char* extension, std::vector<std::string>& out) const
{
    if (Exists(directory) && IsDirectory(directory))
    {
        char** fileList = PHYSFS_enumerateFiles(directory);
        
        unsigned int dirSize = strlen(directory) + 2;
        char* dir = new char[dirSize];
        memcpy(dir, directory, dirSize - 1);
        dir[dirSize - 2] = '/';
        dir[dirSize - 1] = '\0';
        std::string path;
        for (char** file = fileList; *file != nullptr; ++file)
        {
            path.clear();
            path += dir;
            path += *file;
            if (IsDirectory(path.c_str()))
            {
                DiscoverFiles(path.c_str(), extension, out);
                continue;
            }
            char* currExtension = strrchr(*file, '.');
            if (currExtension && strcmp(extension, currExtension) == 0)
                out.push_back(path);
        }

        delete[] dir;
        PHYSFS_freeList(fileList);
    }
}

void ModuleFileSystem::DiscoverFiles(const char* directory, PathNode* parent) const
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
                PathNode* node = new PathNode(path.c_str(), parent);
                parent->mChildren.push_back(node);
                DiscoverFiles(path.c_str(), node);
                path = directory + std::string("/");
            }
            else
            {
                std::string fileName;
                std::string extensionName;
                SplitPath(path.c_str(), &fileName, &extensionName);
                std::string combine = fileName + extensionName;
                if (!(strcmp(extensionName.c_str(), ".emeta") == 0) && !(strcmp(extensionName.c_str(), ".bin") == 0))
                {   
                    AssetDisplay* assetDisplay = new AssetDisplay(combine.c_str(), path.c_str(), parent);
                    parent->assets.push_back(assetDisplay);
                }        
                path = directory + std::string("/");
            }
        }


        PHYSFS_freeList(fileList);
    }
}

void ModuleFileSystem::NormalizePath(char* path) const
{ 
    while (*path != '\0')
    {
        if (*path == '\\')
        {
            *path = '/';
        }
        ++path;
    }
}

const char* ModuleFileSystem::GetFileFromPath(const char* path) const
{
    return nullptr;
}

const char* ModuleFileSystem::GetExtensionFromPath(const char* path) const
{
    return nullptr;
}

const char* ModuleFileSystem::GetFileExtensionFromPath(const char* path) const
{
    return nullptr;
}

void ModuleFileSystem::SplitPath(const char* path, std::string* file, std::string* extension) const
{
    std::string tempPath = path;
    NormalizePath(tempPath.data());

    unsigned int lastSlashPos = tempPath.find_last_of('/');
    unsigned int dotPos = tempPath.find_last_of('.');

    if(file != nullptr)
        *file = (lastSlashPos < tempPath.length()) ? tempPath.substr(lastSlashPos + 1, dotPos - lastSlashPos - 1) : tempPath.substr(0, dotPos);

    if(extension != nullptr)
        *extension = (dotPos < tempPath.length()) ? tempPath.substr(dotPos) : tempPath;
}

void ModuleFileSystem::GetDirectoryFiles(const char* directory, std::vector<std::string>& files) const
{
    char** dirFiles = PHYSFS_enumerateFiles("Assets/Scripts");
    for (char** file = dirFiles; *file != nullptr; ++file)
    {
        files.push_back(*file);
    }

    PHYSFS_freeList(dirFiles);
}

//void ModuleFileSystem::CleanNode(PathNode* node)
//{
//
//    for (int i = 0; i < node->assets.size(); ++i) 
//    {
//        delete node->assets[i];
//    }
//
//    for (int i = 0; i < node->mChildren.size(); ++i) 
//    {
//        CleanNode(node->mChildren[i]);
//    }
//
//    delete node->mName;
//    delete node;
//    node = nullptr;
//}

PathNode::PathNode(const char* name, PathNode* parent) : mParent(parent)
{
    unsigned int size = strlen(name) + 1;
    mName = new char[size];
    strcpy_s(const_cast<char*>(mName), size, name);
}


AssetDisplay::AssetDisplay(const char* name, const char* path, PathNode* parent) : mParent(parent)
{
    unsigned int sizeName = strlen(name) + 1;
    mName = new char[sizeName];
    strcpy_s(const_cast<char*>(mName), sizeName, name);

    unsigned int sizePath = strlen(path) + 1;
    mPath = new char[sizePath];
    strcpy_s(const_cast<char*>(mPath), sizePath, path);
}