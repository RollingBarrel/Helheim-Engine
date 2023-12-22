#pragma once
#include "Panel.h"
#define INSPECTORPANEL "Inspector##"

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();

	void Draw(int windowFlags) override;
};
