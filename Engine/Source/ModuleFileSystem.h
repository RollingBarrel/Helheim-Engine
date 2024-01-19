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

#include <vector>
#include <string>

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

	bool AddToSearchPath(const char* path);

	const char* GetBaseDirectory() const;
	const char* GetWriteDirectory() const;

	void DiscoverFiles(const char* directory, std::vector<std::string>& files, std::vector<std::string>& directories) const;

	const char* NormalizePath(const char* path) const;

	const char* GetFileFromPath(const char* path) const;
	const char* GetExtensionFromPath(const char* path) const;
	const char* GetFileExtensionFromPath(const char* path) const;
	void SplitPath(const char* path, std::string* file = nullptr, std::string* extension = nullptr) const;

private:

};

