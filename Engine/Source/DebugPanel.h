#pragma once

#include "Panel.h"
#define DEBUGPANEL "Debugger"

enum class RenderMode {
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
	RenderMode GetRenderMode() const { return mRenderMode; }

private:
	bool mDrawRaycast = false;
	RenderMode mRenderMode = RenderMode::Shaded;
};

