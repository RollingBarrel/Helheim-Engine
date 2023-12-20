#pragma once
#include "Panel.h"

class ConsolePanel : public Panel
{
public:
	ConsolePanel();
	~ConsolePanel();

	void Draw(int windowFlags) override;
};