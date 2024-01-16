#pragma once

#include "Panel.h"
#define DEBUGPANEL "Quadtree Info"

class DebugPanel : public Panel
{
public:
	DebugPanel();
	~DebugPanel();

	void Draw(int windowFlags) override;

};

