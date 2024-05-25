#pragma once

#include "Panel.h"
#include "GameObject.h"

#define DEBUGPANEL "Debug"

enum class RenderMode 
{
	Shaded,
	Wireframe,
	ShadedWireframe
};

class DebugPanel : public Panel
{
public:
	DebugPanel();
	~DebugPanel();

	void Draw(int windowFlags) override;
	bool ShouldDrawBoundingBoxes() const { return mDrawBoundingBoxes; }
	RenderMode GetRenderMode() const { return mRenderMode; }

private:
	bool mDrawRaycast = false;
	bool mDrawBoundingBoxes = true;
	bool mShowFpsOnEditor = false;
	RenderMode mRenderMode = RenderMode::Shaded;

	void SetShouldDrawForAll(GameObject* root, bool shouldDraw);
	int GetTotalTriangleCount(GameObject* root);
};

