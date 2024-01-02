#pragma once
#include "Module.h"

#define LIBRARY_PATH "Library/"
#define LIBRARY_MESH_PATH "Library/Meshes/"
#define LIBRARY_TEXTURE_PATH "Library/Textures/"
#define LIBRARY_MATERIAL_PATH "Library/Materials/"

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

private:

	bool CreateDirectory(const char* directory);

};

