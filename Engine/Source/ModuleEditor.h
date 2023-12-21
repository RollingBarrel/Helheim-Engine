#pragma once
#include "Module.h"
#include <vector>

struct ImGuiIO;

class Panel;
class AboutPanel;
class ConsolePanel;
class InspectorPanel;

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

	const InspectorPanel* GetInspectorPanel() const { return mInspector; }

private:
	ImGuiIO* io;

	std::vector<Panel*> mPanels;

	AboutPanel* mAbout = nullptr;
	ConsolePanel* mConsole = nullptr;
	InspectorPanel* mInspector = nullptr;
};

