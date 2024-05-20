#include "Debug.h"
#include "Math/float3.h"
#include "Application.h"
#include "ModuleDebugDraw.h"


void Debug::DrawLine(const float3& start, const float3& end, float3 color, float duration, bool depthTest)
{
	App->GetDebugDraw()->DrawLine(start, end, color, duration, depthTest);
}

void Debug::DrawCube(const OBB& obb, const float3& color)
{
	App->GetDebugDraw()->DrawCube(obb, color);
}
