#pragma once
#include "Module.h"
#include <vector>

struct ImGuiIO;

class Panel;
class AboutPanel;
class ConsolePanel;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

private:
	ImGuiIO* io;

	std::vector<Panel*> mPanels;

	AboutPanel* mAbout = nullptr;
	ConsolePanel* mConsole = nullptr;
};

