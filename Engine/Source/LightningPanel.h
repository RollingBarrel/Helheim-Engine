#pragma once
#include "Panel.h"
#define LIGHTNINGPANEL "LightningPanel##"

class LightningPanel : public Panel
{
public:
	LightningPanel();
	void Draw(int windowFlags) override;
};

