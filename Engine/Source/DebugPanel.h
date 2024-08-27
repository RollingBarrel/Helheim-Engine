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
	bool ShouldDrawColliders() const { return mDrawColliders; }
	bool ShouldDrawBoundingBoxes() const { return mBoundingBoxes; }
	RenderMode GetRenderMode() const { return mRenderMode; }

private:
	bool mDrawRaycast = false;
	bool mDrawColliders = false;
	bool mBoundingBoxes = false;
	bool mShowFpsOnEditor = false;
	RenderMode mRenderMode = RenderMode::Shaded;

	void SetShouldDrawForAll(GameObject* root, bool shouldDraw);
	void GetCountInfo(const GameObject& root, unsigned int* vertices = nullptr, unsigned int* indices = nullptr, unsigned int* tris = nullptr);
};

