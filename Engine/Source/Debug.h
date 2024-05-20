#pragma once

#include "MathGeoLibFwd.h"

class ENGINE_API Debug
{
public:

	static void DrawLine(const float3& start, const float3& end, float3 color, float duration = 0.0f, bool depthTest = true);
	static void DrawCube(const OBB& obb, const float3& color);
};

