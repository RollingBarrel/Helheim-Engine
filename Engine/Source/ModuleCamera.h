#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

class ModuleCamera : public Module
{
public:
	bool Init() override;
	update_status Update(float dt) override;

	void LookAt(float3 eyePos, float3 targetPos, float3 upVector);
	void Transform(float3 vec);
	void Rotate(const float3& axix, float angleRad);
	const float3& GetPos() const { return frustum.pos; }
	float4x4 GetViewMatrix() const { return frustum.ViewMatrix(); }
	float4x4 GetProjectionMatrix() const { return frustum.ProjectionMatrix(); }
	float4x4 GetViewProjMatrix() const { return frustum.ViewProjMatrix(); }
	unsigned int GetCameraUniffromsId() const { return cameraUnis; }
	void WindowResized(int w, int h);

	const Frustum& GetFrustum() const { return frustum; }

private:
	Frustum frustum;
	unsigned int cameraUnis = 0;
};

#endif /* _MODULE_CAMERA_H_ */
