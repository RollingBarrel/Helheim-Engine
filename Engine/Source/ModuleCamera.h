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

	const float3& GetPos() const { return mFrustum->pos; }
	float4x4 GetViewMatrix() const { return mFrustum->ViewMatrix(); }
	float4x4 GetProjectionMatrix() const { return mFrustum->ProjectionMatrix(); }
	float4x4 GetViewProjMatrix() const { return mFrustum->ViewProjMatrix(); }
	void WindowResized(int w, int h);
	void CheckRaycast();
	void DrawRayCast(bool draw) { mDrawRayCast = draw; }
	const Frustum* GetFrustum() const { return mFrustum; }
	bool CleanUp() override;

private:
	Frustum* mFrustum = nullptr;
	Ray mRay;
	bool mDrawRayCast; 
	std::map<float, GameObject*> mIntersectMap;
};

#endif /* _MODULE_CAMERA_H_ */
