#pragma once

#include "Panel.h"

#include <string>
#include <vector>

#include "imgui.h"

#define SETTINGSPANEL "Settings##"

struct WindowState {
	std::string name;
	bool IsOpen;
	ImVec2 position;
	ImVec2 size;
};

class SettingsPanel : public Panel
{
public:
	SettingsPanel();
	~SettingsPanel();

	void Draw(int windowFlags) override;
private:
	bool culling = false;
	bool engineVsyncEnabled = false;
	bool gameVsyncEnabled = false;
	bool engineFpsLimitEnabled = false;
	bool gameFpsLimitEnabled = false;
	int engineFpsLimit = 0;
	int gameFpsLimit = 0;
	bool grid = false;

	std::vector<WindowState*> mOpenedWindowsInfo;

	void SaveSettings();
	void LoadSettings();

};

