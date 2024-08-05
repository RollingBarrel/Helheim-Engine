#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include <vector>

class CameraComponent;
class GameObject;

class ENGINE_API ModuleCamera : public Module
{
public:
	void ActivateFirstCamera() { if(!mActiveCameras.empty()) mCurrentCamera = mActiveCameras.front(); }
	virtual bool AddEnabledCamera(CameraComponent* camera);
	virtual bool RemoveEnabledCamera(CameraComponent* camera);
	bool AddMainCamera(CameraComponent* camera);

	const virtual CameraComponent* GetCurrentCamera() const;
	void SetAspectRatio(float aspectRatio);

protected:
	bool RemoveMainCamera(CameraComponent* camera);

	std::vector<CameraComponent*> mActiveCameras;
	CameraComponent* mMainCamera = nullptr;
	CameraComponent* mCurrentCamera = nullptr;
};

#endif /* _MODULE_CAMERA_H_ */
