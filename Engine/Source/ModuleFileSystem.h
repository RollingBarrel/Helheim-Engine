#pragma once
#include "Module.h"

#define LIBRARY_PATH "Library/"
#define LIBRARY_MESH_PATH "Library/Meshes/"
#define LIBRARY_TEXTURE_PATH "Library/Textures/"
#define LIBRARY_MATERIAL_PATH "Library/Materials/"
#define LIBRARY_SHADER_PATH "Library/Shaders/"

#define ASSETS_PATH "Assets/"
#define ASSETS_MODEL_PATH "Assets/Models/"
#define ASSETS_TEXTURE_PATH "Assets/Textures/"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem();

	bool Init() override;
	//update_status PreUpdate() override;
	update_status Update() override;
	//update_status PostUpdate() override;
	bool CleanUp() override;

	unsigned int Load(const char* filePath, char** buffer) const;
	unsigned int Save(const char* filePath, const void* buffer, unsigned int size, bool append = false) const;

	bool Copy(const char* sourceFilePath, const char* destinationFilePath);
	bool Delete(const char* filePath);
	bool CreateDirectory(const char* directory);
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;

private:



};

