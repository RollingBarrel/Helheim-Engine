#ifndef _SCENE_PANEL_H_
#define _SCENE_PANEL_H_
#include "Panel.h"
#define SCENEPANEL "Scene##"
#include "imgui.h"
#include "ImGuizmo.h"

class ScenePanel : public Panel
{
public:
	ScenePanel();
	~ScenePanel();
	bool isHovered() { return isWindowsHovered; }
	void Draw(int windowFlags) override;
private:
	unsigned int prevSizeX = 0;
	unsigned int prevSizeY = 0;
	bool isWindowsHovered = false;

	//ImGuizmo variables
	ImGuizmo::OPERATION mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mCurrentGuizmoMode = ImGuizmo::LOCAL;
	bool useSnap = false;
	float snap[3] = { 1.f, 1.f, 1.f };
};
#endif /* _SCENE_PANEL_H_ */