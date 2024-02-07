#pragma once
#include "Geometry/Frustum.h"
#include <memory>
class CameraUtils
{
public:
	static Frustum* InitiateCamera();
	static void LookAt(float3 eyePos, float3 targetPos, float3 upVector, Frustum& camera);
	static void Rotate(const float3& axis, float angleRad, Frustum& camera);
	static void Transform(float3 vec, Frustum& camera);

};

