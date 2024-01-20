#pragma once

#include "Panel.h"
#define DEBUGPANEL "Debugger"

class DebugPanel : public Panel
{
public:
	DebugPanel();
	~DebugPanel();

	void Draw(int windowFlags) override;

};

