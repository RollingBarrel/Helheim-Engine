#ifndef _SCENE_PANEL_H_
#define _SCENE_PANEL_H_
#include "Panel.h"
#define SCENEPANEL "Scene##"

#include "imgui.h"
#include "Math/float2.h"

class ScenePanel : public Panel
{
public:
	ScenePanel();
	~ScenePanel();
	bool isHovered() { return isWindowsHovered; }
	void Draw(int windowFlags) override;

	const float2& GetWindowsPos() { return mWindowsPosition; };
	const float2& GetWindowsSize() { return mWindowsSize; }
	const float2& GetMousePosition() { return mMousePosition; }
	bool IsGuizmoUsing() { return mIsGuizmoUsing; }

private:

	void DrawScene();

	unsigned int prevSizeX = 0;
	unsigned int prevSizeY = 0;
	bool isWindowsHovered = false;

	float2 mWindowsPosition;
	float2 mWindowsSize;
	float2 mMousePosition;
  
	bool mIsGuizmoUsing = false;
};
#endif /* _SCENE_PANEL_H_ */