#pragma once

#include "Panel.h"

#include <string>
#include <vector>

#include "imgui.h"

#define SETTINGSPANEL "Settings##"

struct WindowState 
{
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

	void LoadSettings();
	void Draw(int windowFlags) override;
private:
	bool mCulling = false;
	bool mEngineVsyncEnabled = false;
	bool mGameVsyncEnabled = false;
	bool mEngineFpsLimitEnabled = false;
	bool mGameFpsLimitEnabled = false;
	int mEngineFpsLimit = 0;
	int mGameFpsLimit = 0;
	bool mGrid = true;

	std::vector<WindowState*> mOpenedWindowsInfo;

	void SaveSettings();
};

