#pragma once

#include "Panel.h"
#include "GameObject.h"

#define DEBUGPANEL "Debugger"

class DebugPanel : public Panel
{
public:
	DebugPanel();
	~DebugPanel();

	void Draw(int windowFlags) override;
	bool ShouldDrawColliders() const { return mDrawColliders; }

private:
	bool mDrawRaycast = false;
	bool mDrawColliders = false;
	bool mShowFpsOnEditor = false;
	int mRenderMode = 0;

	void SetShouldDrawForAll(GameObject* root, bool shouldDraw);
};

