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
	std::string mSkyboxFilePath = "Assets/Textures/rural_asphalt_road_4k.hdr";
	int mIrradianceSize = 256;
	int mSpecPrefilteredSize = 256;
	int mSpecEnvBRDFSize = 512;
};

