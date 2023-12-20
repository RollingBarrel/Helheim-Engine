#pragma once
#include "Panel.h"

class AboutPanel : public Panel
{
public:
	AboutPanel();
	~AboutPanel();

	void Draw(int windowFlags) override;
};

