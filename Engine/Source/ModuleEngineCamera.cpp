#include "ModuleEngineCamera.h"
#include "EngineApp.h"
#include "GameObject.h"
#include "CameraComponent.h"
#include "Physics.h"
#include "Quadtree.h"

#include "ModuleOpenGL.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"

#include "Geometry/Ray.h"
#include "float3x3.h"

bool ModuleEngineCamera::Init()
{
	mEditorCameraGameObject = new GameObject(nullptr);
	mEditorCamera = reinterpret_cast<CameraComponent*>(mEditorCameraGameObject->CreateComponent(ComponentType::CAMERA));
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
	App->GetOpenGL()->SetOpenGlCameraUniforms();
	LOG("X: %f, Y: %f, Z: %f", mEditorCameraGameObject->GetRotation().x, mEditorCameraGameObject->GetRotation().y, mEditorCameraGameObject->GetRotation().z);
	return UPDATE_CONTINUE;
}


bool ModuleEngineCamera::CleanUp()
{
	if (mEditorCamera)
	{
		delete mEditorCamera;
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
	if (mIsEditorCameraActive)
	{
		mCurrentCamera = mEditorCamera;
		App->GetOpenGL()->SetOpenGlCameraUniforms();
	}

	return removed;
}

void ModuleEngineCamera::SetEditorCameraPosition(float3 position)
{
	mEditorCamera->SetPos(position);
}

void ModuleEngineCamera::SetEditorCameraFrontUp(float3 front, float3 up)
{
	mEditorCamera->SetFrontUp(front, up);
}

void ModuleEngineCamera::MousePicking(Ray& ray)
{
	ray = Physics::ScreenPointToRay(App->GetInput()->GetGlobalMousePosition());

	bool intersects = false;
	bool intersectsTriangle = false;

	Quadtree* root = App->GetScene()->GetQuadtreeRoot();

	if (!reinterpret_cast<ScenePanel*>(EngineApp->GetEditor()->GetPanel(SCENEPANEL))->IsGuizmoUsing())
	{
		std::map<float, Hit> hits = root->RayCast(&ray);
		if (!hits.empty())
		{
			std::pair<const float, Hit> intersectGameObjectPair = *hits.begin();
			if (intersectGameObjectPair.second.mGameObject != nullptr)
			{
				GameObject* parentGameObject = intersectGameObjectPair.second.mGameObject;
				while (!parentGameObject->GetParent()->IsRoot())
				{
					parentGameObject = parentGameObject->GetParent();
				}

				GameObject* focusedGameObject = reinterpret_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
				if (focusedGameObject->GetID() == parentGameObject->GetID())
				{
					reinterpret_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(intersectGameObjectPair.second.mGameObject);
				}
				else
				{
					reinterpret_cast<HierarchyPanel*>(EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(parentGameObject);
				}
			}
		}
	}

}

void ModuleEngineCamera::CameraControls(float dt)
{
	bool hasBeenUpdated = false;
	static float shiftSpeed = 3.0f;
	static Ray mousePickingRay;
	if (mDrawRayCast)
	{   //TODO: FIX DEBUG DRAW NOT BEING CORRECT
		EngineApp->GetDebugDraw()->DrawLine(mousePickingRay.pos, mousePickingRay.dir, float3(1.0f, 0.0f, 0.0f));
	}
	if (mIsEditorCameraActive && ((ScenePanel*)EngineApp->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
	{
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
			mCurrentCamera->Transform(float3(0, 0, speed * 10.f * App->GetInput()->GetMouseWheelMotion()));
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			hasBeenUpdated = true;
			App->GetInput()->GetMouseMotion(mX, mY);
			//mCurrentCamera->Rotate(float3::unitY, -mX * rotateCameraVel);
			//mCurrentCamera->Rotate(mCurrentCamera->GetFrustum().WorldRight(), -mY * rotateCameraVel);

			float3x3 rotationX = float3x3::RotateAxisAngle(mEditorCameraGameObject->GetRight(), DegToRad(mY));
			float3x3 rotationY = float3x3::RotateAxisAngle(float3::unitY, DegToRad(-mX));
			float3x3 rotation = rotationY.Mul(rotationX);


			Quat quatoriginal = mEditorCameraGameObject->GetRotationQuat();
			Quat newQuat = Quat(rotation);
			newQuat =  newQuat * quatoriginal;
			float3 eulerrot = newQuat.ToEulerXYZ();
			mEditorCameraGameObject->SetRotation(eulerrot);


			if (App->GetInput()->GetKey(KeyboardKeys_Q) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(0, -dtSpeed, 0));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetUp() * -dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_E) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(0, dtSpeed, 0));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetUp() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_W) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(0, 0, dtSpeed));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetFront() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_S) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(0, 0, -dtSpeed));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetFront() * -dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_A) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(-dtSpeed, 0, 0));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetRight() * dtSpeed);
			}

			if (App->GetInput()->GetKey(KeyboardKeys_D) == KeyState::KEY_REPEAT)
			{
				//mCurrentCamera->Transform(float3(dtSpeed, 0, 0));
				mEditorCameraGameObject->Translate(mEditorCameraGameObject->GetRight() * -dtSpeed);
			}
		}
		
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			MousePicking(mousePickingRay);
		}
		
		//paning
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			mEditorCameraGameObject->Translate(float3(mX * speed, 0, 0));
			mEditorCameraGameObject->Translate(float3(0, mY * speed, 0));
		}
		
		/*
		//orbiting camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(KeyboardKeys_LALT) == KeyState::KEY_REPEAT)
		{
			float3 focus = mCurrentCamera->GetFrustum().pos;
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);

			if (mX != 0)
			{
				float3x3 rotationMatrixX = float3x3::RotateAxisAngle(mCurrentCamera->GetFrustum().up, -mX * rotateCameraVel);
				focus = rotationMatrixX.Mul(focus);
			}
			if (mY != 0)
			{
				float3x3 rotationMatrixY = float3x3::RotateAxisAngle(mCurrentCamera->GetFrustum().WorldRight(), mY * rotateCameraVel);
				focus = rotationMatrixY.Mul(focus);
			}

			GameObject* focusedObject = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
			if (focusedObject)
				mCurrentCamera->LookAt(focus, focusedObject->GetPosition(), float3::unitY);
		}
		*/
		/*
		if (App->GetInput()->GetKey(KeyboardKeys_F) == KeyState::KEY_DOWN)
		{

			GameObject* selectedGameObject = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
			float3 selectedObjectPosition;
			if (selectedGameObject)
			{
				selectedObjectPosition = selectedGameObject->GetPosition();
			}
			float3 initialCameraPosition = mEditorCameraGameObject->GetPosition();
			
			float desiredDistance = 5.0f;
			float3 finalCameraPosition = selectedObjectPosition - (desiredDistance * (selectedObjectPosition - initialCameraPosition).Normalized());
			mCurrentCamera->SetPos(finalCameraPosition);
			mCurrentCamera->LookAt(mCurrentCamera->GetFrustum().pos, selectedObjectPosition, float3::unitY);
		}
		*/
	}

}



