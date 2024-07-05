#ifndef _SCENE_PANEL_H_
#define _SCENE_PANEL_H_
#include "Panel.h"
#define SCENEPANEL "Scene##"

#include "Math/float2.h"

class AnimationComponent;

class ScenePanel : public Panel
{
public:
	ScenePanel();
	~ScenePanel();
	bool isHovered() { return isWindowsHovered; }
	void Draw(int windowFlags) override;

	const float2& GetWindowsPos() { return mWindowsPosition; };
	const float2& GetWindowsSize() { return mWindowsSize; }
	bool IsGuizmoUsing() { return mIsGuizmoUsing; }

private:

	void MenuGBuffer();
	void DrawScene();

	unsigned int prevSizeX = 0;
	unsigned int prevSizeY = 0;
	bool isWindowsHovered = false;

	float2 mWindowsPosition;
	float2 mWindowsSize;
  
	bool mIsGuizmoUsing = false;
	unsigned int currentScene = 0;
	std::string currentSceneName;
};
#endif /* _SCENE_PANEL_H_ */