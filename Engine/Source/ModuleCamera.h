#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"
#include <map>

class Quadtree;
class GameObject;

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
	void DrawRayCast(bool draw) { mDrawRayCast = draw; }
	void WindowResized(int w, int h);
	void CheckRaycast();
	const Frustum& GetFrustum() const { return frustum; }
	void QuadTreeRaycast(const Quadtree* quadtree);


private:
	Frustum frustum;
	Ray mRay;
	bool mDrawRayCast; 
	std::map<float, GameObject*> mIntersectMap;
};

#endif /* _MODULE_CAMERA_H_ */
