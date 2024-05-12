#pragma once
#include "ModuleCamera.h"
#include "float3.h"

class ModuleEngineCamera : public ModuleCamera
{
	friend class SettingsPanel;
public:
	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;


	void ActivateEditorCamera();
	void ActivateGameCamera();

	bool AddEnabledCamera(CameraComponent* camera) override;
	bool RemoveEnabledCamera(CameraComponent* camera) override;

	const CameraComponent* GetEditorCamera() const { return mEditorCamera; }
	bool IsEditorCameraActive() { return mIsEditorCameraActive; }
	void DrawRaycast(bool draw) { mDrawRayCast = draw; }

private:
	void MousePicking(Ray& ray);
	void CameraControls(float dt);

	GameObject* mEditorCameraGameObject = nullptr;
	CameraComponent* mEditorCamera = nullptr;

	bool mIsEditorCameraActive = true;
	bool mDrawRayCast = false;
};

