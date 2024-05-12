#pragma once
#include "ModuleCamera.h"
#include "float3.h"

class ModuleEngineCamera : public ModuleCamera
{

public:
	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;


	void ActivateEditorCamera();
	void ActivateGameCamera();

	bool AddEnabledCamera(CameraComponent* camera) override;
	bool RemoveEnabledCamera(CameraComponent* camera) override;

	CameraComponent* GetEditorCamera() { return mEditorCamera; }
	bool IsEditorCameraActive() { return mIsEditorCameraActive; }
	void DrawRaycast(bool draw) { mDrawRayCast = draw; }

	void SetEditorCameraPosition(float3 position);
	void SetEditorCameraFrontUp(float3 front, float3 up);

private:
	void MousePicking(Ray& ray);
	void CameraControls(float dt);

	GameObject* mEditorCameraGameObject;
	CameraComponent* mEditorCamera = nullptr;

	bool mIsEditorCameraActive = true;
	bool mDrawRayCast = false;
};

