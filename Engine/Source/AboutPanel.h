#pragma once
#include "Panel.h"
#define ABOUTPANEL "About##"

class AboutPanel : public Panel
{
public:
	AboutPanel();
	~AboutPanel();

	void Draw(int windowFlags) override;
};

