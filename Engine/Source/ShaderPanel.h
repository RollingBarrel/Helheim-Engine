#pragma once
#include "Panel.h"
#define SHADERPANEL "Shader##"

class ShaderPanel : public Panel
{
public:
	ShaderPanel();
	~ShaderPanel();

	void Draw(int windowFlags) override;
};
