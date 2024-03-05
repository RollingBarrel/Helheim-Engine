#pragma once
#include "Panel.h"
#define SKYBOXPANEL "Skybox##"
class SkyboxPanel : public Panel
{
public:
	SkyboxPanel();
	void Draw(int windowFlags = 0);
};

