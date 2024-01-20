#pragma once

#include "Panel.h"
#define LOADSCENEPANEL "Load Scene"

class LoadScenePanel : public Panel
{
public:
	LoadScenePanel();
	~LoadScenePanel();

	void Draw(int windowFlags) override;

};