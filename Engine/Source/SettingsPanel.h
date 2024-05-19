#pragma once

#include "Panel.h"

#include <string>
#include <vector>

#include "imgui.h"

#define SETTINGSPANEL "Settings##"

struct WindowState 
{
	std::string mName;
	bool mIsOpen = false;
	ImVec2 mPosition;
	ImVec2 mSize;
};

class SettingsPanel : public Panel
{
public:
	SettingsPanel();
	~SettingsPanel();

	void SaveSettings();
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

};

