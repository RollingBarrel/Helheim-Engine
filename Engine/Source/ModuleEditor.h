#ifndef _MODULE_EDITOR_H_
#define _MODULE_EDITOR_H_

#include "Module.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <map>

struct ImGuiIO;
class Panel;

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
	Panel* GetPanel(const char* name) { return mPanels[name]; }

	void ShowMainMenuBar();
	void ResetFloatingPanels(bool openPanels);
	

private:
	ImGuiIO* io = nullptr;
	bool loadSceneOpen = false;
	std::map<const char*, Panel*> mPanels;
	void OpenLoadScene();

	//ImGuizmo variables
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
};

#endif /* _MODULE_EDITOR_H_ */
