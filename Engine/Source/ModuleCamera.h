#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"
#include <map>

class CameraComponent;
class GameObject;

class ModuleCamera : public Module
{
public:
	bool Init() override;
	update_status Update(float dt) override;


	//void WindowResized(int w, int h);
	void CheckRaycast();
	void DrawRayCast(bool draw) { mDrawRayCast = draw; }
	bool CleanUp() override;

	const void CreateEditorCamera();

	void SetCurrentCamera(GameObject* camera);

	const CameraComponent* GetCurrentCamera() const;

	const CameraComponent* GetEditorCamera() const;

	void ActivateEditorCamera();




private:
	GameObject* mEditorCamera = nullptr;
	GameObject* mCurrentCamera = nullptr;

	CameraComponent* mCurrentCameraComponent = nullptr;

	Ray mRay;
	bool mDrawRayCast = false; 
	std::map<float, GameObject*> mIntersectMap;
};

#endif /* _MODULE_CAMERA_H_ */
