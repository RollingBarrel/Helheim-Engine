#pragma once
#include "Panel.h"
#define RESOURCEPANEL "Resource##"


class ResourcePanel : public Panel
{
public:
	ResourcePanel();

	void Draw(int windowFlags) override;
};

