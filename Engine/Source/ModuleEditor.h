#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"


#include <map>
#include <vector>

struct ImGuiIO;
class Panel;
class OptickAdapter;

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
	Panel* GetPanel(const char* name) { return mPanels[name]; }
	std::map<const char*, Panel*> GetPanelList() { return mPanels; }
	std::vector<const char*> GetPanelNames() { return mPanelNames; }	
	void OpenPanel(const char* name, const bool focus);
	void SaveUserSettings();
	void ShowMainMenuBar();
	void ResetFloatingPanels(bool openPanels);

private:
	ImGuiIO* io = nullptr;
	bool mLoadSceneOpen = false;
	bool mSaveSceneOpen = false;
	std::map<const char*, Panel*> mPanels;
	std::vector<const char*> mPanelNames;
	OptickAdapter* mOptick = nullptr;
	void OpenLoadScene();
	void OpenSaveScene();
	void Style();
};

#endif /* _MODULE_EDITOR_H_ */
