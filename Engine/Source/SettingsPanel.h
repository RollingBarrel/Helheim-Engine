#pragma once

#include "Panel.h"
#define SETTINGSPANEL "Settings##"

class SettingsPanel : public Panel
{
public:
	SettingsPanel();
	~SettingsPanel();

	void Draw(int windowFlags) override;
};

