#pragma once
#include "Module.h"
#include <vector>
#include <map>

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
	bool IsPanelOpen(const char* name);

private:
	void AddPanel(Panel* panel, const char* name);

	ImGuiIO* io;

	std::map<const char*, Panel*> mPanelsMap;
	std::vector<Panel*> mPanels;

	AboutPanel* mAbout = nullptr;
	ConsolePanel* mConsole = nullptr;
};

