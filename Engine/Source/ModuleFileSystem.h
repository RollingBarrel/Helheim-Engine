#pragma once
#include "Module.h"

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

};

