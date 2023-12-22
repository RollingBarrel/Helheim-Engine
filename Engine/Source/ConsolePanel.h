#pragma once
#include "Panel.h"
#define CONSOLEPANEL "Console##"

class ConsolePanel : public Panel
{
public:
	ConsolePanel();
	~ConsolePanel();

	void Draw(int windowFlags) override;
};