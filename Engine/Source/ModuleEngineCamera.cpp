#include "ModuleEngineCamera.h"
#include "EngineApp.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Physics.h"

#include "Geometry/AABB.h"
#include "Geometry/Sphere.h"
#include "Math/MathFunc.h"

#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "ModuleEditor.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"

bool ModuleEngineCamera::Init()
{
	mEditorCameraGameObject = new GameObject("EditorCamera", nullptr);
	mEditorCamera = static_cast<CameraComponent*>(mEditorCameraGameObject->CreateComponent(ComponentType::CAMERA));
	mActiveCameras.clear();
	if (App != nullptr)
	{
		float w = EngineApp->GetWindow()->GetWidth();
		float h = EngineApp->GetWindow()->GetHeight();

		mEditorCamera->SetAspectRatio(w / h);
	}
	mCurrentCamera = mEditorCamera;

	return true;
}

update_status ModuleEngineCamera::Update(float dt)
{
	CameraControls(dt);
	mEditorCameraGameObject->Update();
	//LOG("X: %f, Y: %f, Z: %f", RadToDeg(mEditorCameraGameObject->GetRotation().x), RadToDeg(mEditorCameraGameObject->GetRotation().y), RadToDeg(mEditorCameraGameObject->GetRotation().z));
	//LOG("X: %f, Y: %f, Z: %f", mEditorCameraGameObject->GetRotation().x, mEditorCameraGameObject->GetRotation().y, mEditorCameraGameObject->GetRotation().z);
	return UPDATE_CONTINUE;
}


bool ModuleEngineCamera::CleanUp()
{
	if (mEditorCameraGameObject)
	{
		delete mEditorCameraGameObject;
	}

	return true;
}

void ModuleEngineCamera::ActivateEditorCamera()
{
	if (!mCurrentCamera || mCurrentCamera->GetID() != mEditorCamera->GetID())
	{
		mCurrentCamera = mEditorCamera;
		mIsEditorCameraActive = true;
		App->GetOpenGL()->SetOpenGlCameraUniforms();
	}
}

void ModuleEngineCamera::ActivateGameCamera()
{
	if (!mCurrentCamera || mCurrentCamera->GetID() == mEditorCamera->GetID())
	{
		if (mMainCamera)
		{
			mCurrentCamera = mMainCamera;
		}
		else if (!mActiveCameras.empty())
		{
			mCurrentCamera = mActiveCameras.front();
		}
		else
		{
			mCurrentCamera = nullptr;
		}

		mIsEditorCameraActive = false;
		App->GetOpenGL()->SetOpenGlCameraUniforms();
	}
}

bool ModuleEngineCamera::AddEnabledCamera(CameraComponent* camera)
{
	bool added = ModuleCamera::AddEnabledCamera(camera);

	if (mIsEditorCameraActive)
	{
		mCurrentCamera = mEditorCamera;
		App->GetOpenGL()->SetOpenGlCameraUniforms();
	}

	return added;
}

bool ModuleEngineCamera::RemoveEnabledCamera(CameraComponent* camera)
{
	bool removed = ModuleCamera::RemoveEnabledCamera(camera);
	if (mIsEditorCameraActive || !mCurrentCamera )
	{
		mCurrentCamera = mEditorCamera;
		mIsEditorCameraActive = true;
		App->GetOpenGL()->SetOpenGlCameraUniforms();
	}

	return removed;
}

void ModuleEngineCamera::MousePicking(Ray& ray)
{
	ray = Physics::ScreenPointToRay(App->GetInput()->GetLocalMousePosition());

	bool intersects = false;
	bool intersectsTriangle = false;

	if (!static_cast<ScenePanel*>(EngineApp->GetEditor()->GetPanel(SCENEPANEL))->IsGuizmoUsing())
	{
		std::map<float, Hit> hits;
		//root->RayCast(ray, hits); //FIX MOUSE PICKING
		if (!hits.empty())
		{
			std::pair<const float, Hit> intersectGameObjectPair = *hits.begin();
			if (intersectGameObjectPair.second.mGameObject != nullptr)
			{
				const GameObject* parentGameObject = intersectGameObjectPair.second.mGameObject;
				while (!parentGameObject->GetParent()->IsRoot())
				{
					parentGameObject = parentGameObject->GetParent();
				}

				const GameObject* focusedGameObject = static_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
				if (focusedGameObject->GetID() == parentGameObject->GetID())
				{
					static_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(*intersectGameObjectPair.second.mGameObject);
				}
				else
				{
					static_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(*parentGameObject);
				}
			}
		}
	}

}

void ModuleEngineCamera::MouseFix()
{
	float2 mouse = EngineApp->GetInput()->GetLocalMousePosition();
	float2 windowSize = float2(EngineApp->GetWindow()->GetWidth(), EngineApp->GetWindow()->GetHeight());

	float offset = 5.0f;
	if (mouse.x <= 0.0f) 
	{
		EngineApp->GetWindow()->SetMousePositionInWindow(float2(windowSize.x - 5.0f, mouse.y));
		mouse.x = windowSize.x - 5.0f;
	}

	if (mouse.x >= windowSize.x - 1) 
	{
		EngineApp->GetWindow()->SetMousePositionInWindow(float2(5.0f, mouse.y));
		mouse.x = 5.0f;
	}

	if (mouse.y <= 0.0f) 
	{
		EngineApp->GetWindow()->SetMousePositionInWindow(float2(mouse.x, windowSize.y - 5.0f));
		mouse.y = windowSize.y - 5.0f;
	}

	if (mouse.y >= windowSize.y - 1) 
	{
		EngineApp->GetWindow()->SetMousePositionInWindow(float2(mouse.x, 5.0f));
		mouse.y = 5.0f;
	}


}

void ModuleEngineCamera::CameraControls(float dt)
{
	static bool active = false;
	static float shiftSpeed = 3.0f;
	static Ray mousePickingRay;
	if (mDrawRayCast)
	{   //TODO: FIX DEBUG DRAW NOT BEING CORRECT
		EngineApp->GetDebugDraw()->DrawLine(mousePickingRay.pos, mousePickingRay.dir, float3(1.0f, 0.0f, 0.0f));
	}
	bool isSceneHovered = ((ScenePanel*)EngineApp->GetEditor()->GetPanel(SCENEPANEL))->isHovered();
	bool isGuizmoUsing = reinterpret_cast<ScenePanel*>(EngineApp->GetEditor()->GetPanel(SCENEPANEL))->IsGuizmoUsing();
	if ((mIsEditorCameraActive && isSceneHovered && !isGuizmoUsing) || active)
	{
		active = false;
		const float dtTransformCameraVel = dt * 3.f;
		float transformCameraVel = 0.03f;
		const float rotateCameraVel = 0.01f;

		const float dtFastSpeed = dtTransformCameraVel * shiftSpeed;
		const float fastSpeed = transformCameraVel * 3.0f;
		bool shiftPressed = (EngineApp->GetInput()->GetKey(KeyboardKeys_LSHIFT) == KeyState::KEY_REPEAT) || (App->GetInput()->GetKey(KeyboardKeys_RSHIFT) == KeyState::KEY_REPEAT);
		float dtSpeed = shiftPressed ? dtFastSpeed : dtTransformCameraVel;
		float speed = shiftPressed ? fastSpeed : transformCameraVel;

		shiftSpeed = shiftPressed ? shiftSpeed + 0.05 : 5.0f;


		if (App->GetInput()->GetMouseWheelMotion() != 0)
		{
			mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetFront() * speed * 10.f * App->GetInput()->GetMouseWheelMotion());
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			active = true;
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);

			float3x3 rotationX = float3x3::RotateAxisAngle(mEditorCameraGameObject->GetRight(), DegToRad(mY));
			float3x3 rotationY = float3x3::RotateAxisAngle(float3::unitY, DegToRad(-mX));
			float3x3 rotation = rotationY.Mul(rotationX);

			Quat quatOriginal = mEditorCameraGameObject->GetWorldRotation();
			Quat newQuat = Quat(rotation);
			newQuat =  newQuat * quatOriginal;
			//float3 eulerRotation = newQuat.ToEulerXYZ();
			mEditorCameraGameObject->SetWorldRotation(newQuat);

			MouseFix();

			if (App->GetInput()->GetKey(KeyboardKeys_Q) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetUp() * -dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_E) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetUp() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_W) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetFront() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_S) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetFront() * -dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_A) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetRight() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_D) == KeyState::KEY_REPEAT)
			{
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetRight() * -dtSpeed);
			}


		}
		
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			MousePicking(mousePickingRay);
		}
		
		//Camera Paning
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			active = true;
			int mX, mY;
			MouseFix();
			App->GetInput()->GetMouseMotion(mX, mY);
			mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetRight() * mX * speed);
			mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetUp() * mY * speed);
			
		}
		
		
		//Camera Orbit //TODO: FIX MOVEMENT
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(KeyboardKeys_LALT) == KeyState::KEY_REPEAT)
		{
			GameObject* focusedObject = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
			if (focusedObject)
			{
				active = true;

				float3 focus = mEditorCameraGameObject->GetWorldPosition();
				int mX, mY;
				EngineApp->GetInput()->GetMouseMotion(mX, mY);

				if (mX != 0)
				{
					float3x3 rotationMatrixX = float3x3::RotateAxisAngle(mEditorCameraGameObject->GetUp(), -mX * rotateCameraVel);
					focus = rotationMatrixX.Mul(focus);
				}
				if (mY != 0)
				{
					float3x3 rotationMatrixY = float3x3::RotateAxisAngle(mEditorCameraGameObject->GetRight(), mY * rotateCameraVel);
					focus = rotationMatrixY.Mul(focus);
				}

				mEditorCameraGameObject->SetWorldPosition(focus);
				mEditorCameraGameObject->LookAt(focusedObject->GetWorldPosition());
				
				MouseFix();
			}	
		}
	}

	if ((mIsEditorCameraActive && !isGuizmoUsing))
	{
		if (!EngineApp->GetEditor()->WantToCaptureKeyboard())
		{
			if (App->GetInput()->GetKey(KeyboardKeys_F) == KeyState::KEY_DOWN)
			{
				GameObject* selectedGameObject = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
				if (selectedGameObject && !selectedGameObject->IsRoot())
				{
					AABB objectAABB = selectedGameObject->GetAABB();

					float distance = 2.5f;
					if (objectAABB.IsFinite())
					{
						Sphere objectSphere = objectAABB.MinimalEnclosingSphere();
						distance *= objectSphere.r;
					}

					float3 selectedObjectPosition = selectedGameObject->GetWorldPosition();
					float3 finalCameraPosition = selectedObjectPosition - (mEditorCameraGameObject->GetFront()).Normalized() * distance;
					mEditorCameraGameObject->SetWorldPosition(finalCameraPosition);
				}

			}
		}
	}
}



