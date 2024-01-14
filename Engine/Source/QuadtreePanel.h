#pragma once

#include "Panel.h"
#define QUADTREEPANEL "Quadtree Info"

class QuadtreePanel : public Panel
{
public:
	QuadtreePanel();
	~QuadtreePanel();

	void Draw(int windowFlags) override;

};

