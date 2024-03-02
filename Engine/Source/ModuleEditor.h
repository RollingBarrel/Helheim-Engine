#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <map>

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

	void ShowMainMenuBar();
	void ResetFloatingPanels(bool openPanels);

private:
	ImGuiIO* io = nullptr;
	bool mLoadSceneOpen = false;
	std::map<const char*, Panel*> mPanels;
	OptickAdapter* mOptick;
	void OpenLoadScene();
};

#endif /* _MODULE_EDITOR_H_ */
