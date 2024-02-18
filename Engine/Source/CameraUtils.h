#pragma once
#include "Geometry/Frustum.h"
class CameraUtils
{
public:
	static Frustum* InitiateCamera(float3 initialPosition);
	static void LookAt(float3 eyePos, float3 targetPos, float3 upVector, Frustum& camera);
	static void Rotate(const float3& axis, float angleRad, Frustum& camera);
	static void Transform(float3 vec, Frustum& camera);
	static void CameraComponentTransform(float3 vec, Frustum& camera);

};

