#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"
#include <map>

#include "CameraComponent.h"
#include "GameObject.h"


class ModuleCamera : public Module
{
public:
	bool Init() override;
	update_status Update(float dt) override;

	const float3& GetPos() const { return mFrustum->pos; }
	float4x4 GetViewMatrix() const { return mFrustum->ViewMatrix(); }
	float4x4 GetProjectionMatrix() const { return mFrustum->ProjectionMatrix(); }
	float4x4 GetViewProjMatrix() const { return mFrustum->ViewProjMatrix(); }
	const Frustum* GetFrustum() const { return mFrustum; }
	
	void SetFrustum(Frustum* frustum) { mFrustum = frustum; }

	void WindowResized(int w, int h);
	void CheckRaycast();
	void DrawRayCast(bool draw) { mDrawRayCast = draw; }
	bool CleanUp() override;

	const void CreateEditorCamera();

	void SetCurrentCamera(GameObject* camera);

	const CameraComponent* GetCurrentCamera() { return (CameraComponent*)mCurrentCamera->GetComponent(ComponentType::CAMERA); }

	const CameraComponent* GetEditorCamera() { return (CameraComponent*)mEditorCamera->GetComponent(ComponentType::CAMERA); }

	void CheckRaycast();
	void DrawRayCast(bool draw) { mDrawRayCast = draw; }





private:
	GameObject* mEditorCamera = nullptr;
	GameObject* mCurrentCamera = nullptr;

	CameraComponent* mCurrentCameraComponent = nullptr;

	Ray mRay;
	bool mDrawRayCast; 
	std::map<float, GameObject*> mIntersectMap;
};

#endif /* _MODULE_CAMERA_H_ */
