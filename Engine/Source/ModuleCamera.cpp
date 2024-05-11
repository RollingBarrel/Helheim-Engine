#include "ModuleCamera.h"
#include "Math/MathConstants.h"
#include "Geometry/LineSegment.h"
#include "EngineApp.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"
#include "ModuleOpenGL.h"
#include "HierarchyPanel.h"
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "Geometry/Triangle.h"
#include "Quadtree.h"
#include "SDL_scancode.h"
#include "GameObject.h"
#include "Quat.h"
#include "Physics.h"
#include "CameraComponent.h"
#include <map>


bool ModuleCamera::Init()
{
	return true;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::SetCurrentCamera(GameObject* camera)
{
	if (camera)
	{
		mCurrentCamera = reinterpret_cast<CameraComponent*>(camera->GetComponent(ComponentType::CAMERA));
	}
}

void ModuleCamera::SetCurrentCamera(CameraComponent* camera)
{
	if (camera)
	{
		mCurrentCamera = camera;
	}
	
}
/*
bool ModuleCamera::AddMainCamera(CameraComponent* camera)
{
	if (!mMainCamera && camera->GetOwner()->GetTag()->GetName().compare("MainCamera") == 0)
	{
		mMainCamera = camera;
		return true;
	}
	return false;
}

bool ModuleCamera::RemoveMainCamera(CameraComponent* camera)
{
	if (mMainCamera->GetID() == camera->GetID())
	{
		mMainCamera = nullptr;
	}
}



bool ModuleCamera::AddEnabledCamera(CameraComponent* camera)
{
	bool found = false;
	for (CameraComponent* cameraComponent : mGameEnabledCameras)
	{
		if (cameraComponent->GetID() == camera->GetID())
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		mGameEnabledCameras.push_back(camera);
		AddMainCamera(camera);
		return true;
	}
	return false;
}

bool ModuleCamera::RemoveEnabledCamera(CameraComponent* camera)
{
	for (std::vector<CameraComponent*>::iterator it = mGameEnabledCameras.begin(); it != mGameEnabledCameras.end(); ++it)
	{
		if ((*it)->GetID() == camera->GetID())
		{
			mGameEnabledCameras.erase(it);
			RemoveMainCamera(camera);

			if (camera->GetID() == mCurrentCamera->GetID())
			{
				if (mMainCamera)
				{
					mCurrentCamera = mMainCamera;
				}
				else if (!mGameEnabledCameras.empty())
				{
					mCurrentCamera = mGameEnabledCameras.front();
				}
				else
				{
					mCurrentCamera = nullptr;
				}
			}

			return true;
		}
	}

	return false;
}
*/


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


update_status ModuleCamera::Update(float dt)
{
	return UPDATE_CONTINUE;
}
