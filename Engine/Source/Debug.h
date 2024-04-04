#pragma once

#include "MathGeoLibFwd.h"

class ENGINE_API Debug
{
public:

	static void DrawLine(float3 start, float3 end, float3 color, float duration = 0.0f, bool depthTest = true);
};

