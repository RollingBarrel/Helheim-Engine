#pragma once

#include "Panel.h"

#include <string>
#include <vector>

#include "imgui.h"

#define SETTINGSPANEL "Settings##"
#define NUM_ENGINE_TAGS 4

class SettingsPanel : public Panel
{
public:
	SettingsPanel();
	~SettingsPanel();

	void Draw(int windowFlags) override;
	
	void SaveUserSettings() const;			//User Settings	(No)
	void LoadUserSettings();

	void AddTag(const char* newTag);
	void DeleteTag(const char* tagToDelete);

	void LoadProjectSettings();		
	void LoadEditorLayout();

	const std::vector<std::string>& GetTags() { return mTags; }

	const void ShowDeleteTagsPopup();
	bool mDeleteTagPopup = false;

private:
	void SaveEditorLayout() const;		//ImGui	(Yes)
			

	void SaveProjectSettings() const;		//Project Settings -> Tags & Layers (Yes)

	bool mEngineVsyncEnabled = false;
	bool mGameVsyncEnabled = false;
	bool mEngineFpsLimitEnabled = false;
	bool mGameFpsLimitEnabled = false;
	int mEngineFpsLimit = 0;
	int mGameFpsLimit = 0;
	bool mGrid = true;

	std::vector<std::string> mTags;

	std::string mTagToDelete;

};

