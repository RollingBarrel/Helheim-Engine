#pragma once
#include "Module.h"

#define LIBRARY_PATH "Library/"
//#define LIBRARY_TEXTURE_PATH "Library/Textures/"
//#define LIBRARY_MESH_PATH "Library/Meshes/"
//#define LIBRARY_BONE_PATH "Library/Bones/"
//#define LIBRARY_ANIMATION_PATH "Library/Animations/"
//#define LIBRARY_MATERIAL_PATH "Library/Materials/"
//#define LIBRARY_MODEL_PATH "Library/Models/"
//#define LIBRARY_SCENE_PATH "Library/Scenes/"
//#define LIBRARY_NAVMESH_PATH "Library/NavMeshes/"
//#define LIBRARY_SHADER_PATH "Library/Shaders/"

#define ASSETS_PATH "Assets/"
#define ASSETS_TEXTURE_PATH "Assets/Textures/"
#define ASSETS_MESH_PATH "Assets/Meshes/"
#define ASSETS_BONE_PATH "Assets/Bones/"
#define ASSETS_ANIMATION_PATH "Assets/Animations/"
#define ASSETS_MATERIAL_PATH "Assets/Materials/"
#define ASSETS_MODEL_PATH "Assets/Models/"
#define ASSETS_SCENES_PATH "Assets/Scenes/"
#define ASSETS_NAVMESH_PATH "Assets/NavMeshes/"


#include <vector>
#include <string>

struct AssetDisplay;

struct PathNode
{
	PathNode(const char* name, PathNode* parent = nullptr);
	const char* mName;
	PathNode* mParent;
	std::vector<PathNode*> mChildren;
	std::vector<AssetDisplay*> assets;
};


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

	bool CopyRelativePath(const char* sourceFilePath, const char* destinationFilePath) const;
	bool CopyAbsolutePath(const char* sourceFilePath, const char* destinationFilePath) const;
	bool DeleteDirectory(const char* filePath) const;
	bool CreateDirectory(const char* directory) const;
	bool RemoveFile(const char* filePath) const;
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;
	//The pointer returned by this function needs to be released !!!
	const char* GetLibraryFile(unsigned int id, bool createDir = false) const;
	int64_t GetLastModTime(const char* file) const;
	int64_t GetCreationTime(const char* file) const;

	bool AddToSearchPath(const char* path);

	const char* GetBaseDirectory() const;
	const char* GetWriteDirectory() const;

	void DiscoverFiles(const char* directory, PathNode* parent) const;
	void DiscoverFiles(const char* directory, const char* extension, std::vector<std::string>& out) const;

	void NormalizePath(char* path) const;

	const char* GetFileFromPath(const char* path) const;
	const char* GetExtensionFromPath(const char* path) const;
	const char* GetFileExtensionFromPath(const char* path) const;
	void SplitPath(const char* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	PathNode* GetRootNode() { return mRoot; }

private:
	PathNode* mRoot = nullptr;
};

