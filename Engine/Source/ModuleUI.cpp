#include "ModuleUI.h"


ModuleUI::ModuleUI() 
{
};

ModuleUI::~ModuleUI() 
{
};

bool ModuleUI::Init() {
	return true;
};

update_status ModuleUI::Update() {
	return UPDATE_CONTINUE;
};

update_status ModuleUI::PostUpdate() {
	return UPDATE_CONTINUE;
};
