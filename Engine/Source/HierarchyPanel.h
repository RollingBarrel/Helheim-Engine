#pragma once
#pragma once
#include "Panel.h"
#define HIERARCHYPANEL "Hierarchy##"

class HierarchyPanel : public Panel
{
public:
	HierarchyPanel();
	//~HierarchyPanel();

	void Draw(int windowFlags) override;
};
