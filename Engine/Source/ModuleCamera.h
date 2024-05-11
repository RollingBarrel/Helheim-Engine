#ifndef _MODULE_CAMERA_H_
#define _MODULE_CAMERA_H_

#include "Module.h"
#include "Math/float3.h"


class CameraComponent;
class GameObject;

class ENGINE_API ModuleCamera : public Module
{
public:
	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	const virtual CameraComponent* GetCurrentCamera() const;
	void SetAspectRatio(float aspectRatio);
	virtual void SetCurrentCamera(GameObject* camera);
	virtual void SetCurrentCamera(CameraComponent* camera);

protected:
	
	CameraComponent* mCurrentCamera = nullptr;
};

#endif /* _MODULE_CAMERA_H_ */
