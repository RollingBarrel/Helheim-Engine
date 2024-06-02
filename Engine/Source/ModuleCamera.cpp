#include "ModuleCamera.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "CameraComponent.h"

bool ModuleCamera::AddMainCamera(CameraComponent* camera)
{
	if (!mMainCamera && camera->GetOwner()->GetTag()->GetName().compare("MainCamera") == 0)
	{
		mMainCamera = camera;
		mCurrentCamera = camera;
		return true;
	}
	return false;
}

bool ModuleCamera::RemoveMainCamera(CameraComponent* camera)
{
	if (mMainCamera && mMainCamera->GetID() == camera->GetID())
	{
		mMainCamera = nullptr;
		return true;
	}
	return false;
}



bool ModuleCamera::AddEnabledCamera(CameraComponent* camera)
{
	bool found = false;
	for (CameraComponent* cameraComponent : mActiveCameras)
	{
		if (cameraComponent->GetID() == camera->GetID())
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		mActiveCameras.push_back(camera);
		AddMainCamera(camera);
		App->GetOpenGL()->SetOpenGlCameraUniforms();
		return true;
	}
	return false;
}

bool ModuleCamera::RemoveEnabledCamera(CameraComponent* camera)
{
	for (std::vector<CameraComponent*>::iterator it = mActiveCameras.begin(); it != mActiveCameras.end(); ++it)
	{
		if ((*it)->GetID() == camera->GetID())
		{
			mActiveCameras.erase(it);
			if (RemoveMainCamera(camera))
			{
				for (CameraComponent* cComponent : mActiveCameras)
				{
					AddMainCamera(cComponent);
				}
			}
			if (camera->GetID() == mCurrentCamera->GetID())
			{
				mCurrentCamera = nullptr;
			}
			
			if (!mMainCamera)
			{
				if (!mActiveCameras.empty())
				{
					mCurrentCamera = mActiveCameras.front();
				}
			}
			//if (mCurrentCamera == nullptr)
			App->GetOpenGL()->SetOpenGlCameraUniforms();
			return true;
		}
	}

	return false;
}



const CameraComponent* ModuleCamera::GetCurrentCamera() const
{
	return mCurrentCamera;
}

void ModuleCamera::SetAspectRatio(float aspectRatio)
{
	if (mCurrentCamera)
	{
		mCurrentCamera->SetAspectRatio(aspectRatio);
	}
}