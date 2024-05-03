#pragma once
#include "Panel.h"
#define LIGHTNINGPANEL "LightningPanel##"
#include <string>

class LightningPanel : public Panel
{
public:
	LightningPanel();
	void Draw(int windowFlags) override;

private:
	std::string mSkyboxFileName;
};

