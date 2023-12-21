#pragma once
#include "Panel.h"

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();

	void Draw(int windowFlags) override;
};
