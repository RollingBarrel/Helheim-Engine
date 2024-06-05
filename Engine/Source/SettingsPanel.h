#pragma once

#include "Panel.h"

#include <string>
#include <vector>

#include "imgui.h"

#define SETTINGSPANEL "Settings##"

class SettingsPanel : public Panel
{
public:
	SettingsPanel();
	~SettingsPanel();

	void Draw(int windowFlags) override;
	
	void SaveUserSettings() const;			//User Settings	(No)
	void LoadUserSettings();

	void LoadProjectSettings();		

	const std::vector<std::string>& GetTags() { return mTags; }

private:
	void SaveEditorLayout() const;		//ImGui	(Yes)
	void LoadEditorLayout();			

	void SaveProjectSettings() const;		//Project Settings -> Tags & Layers (Yes)

	bool mCulling = false;
	bool mEngineVsyncEnabled = false;
	bool mGameVsyncEnabled = false;
	bool mEngineFpsLimitEnabled = false;
	bool mGameFpsLimitEnabled = false;
	int mEngineFpsLimit = 0;
	int mGameFpsLimit = 0;
	bool mGrid = true;

	std::vector<std::string> mTags;
};

