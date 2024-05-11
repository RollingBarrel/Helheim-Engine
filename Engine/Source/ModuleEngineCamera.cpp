#include "ModuleEngineCamera.h"
#include "EngineApp.h"
#include "GameObject.h"
#include "CameraComponent.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ScenePanel.h"
#include "HierarchyPanel.h"

bool ModuleEngineCamera::Init()
{
	mEditorCameraGameObject = new GameObject(nullptr);
	mEditorCamera = reinterpret_cast<CameraComponent*>(mEditorCameraGameObject->CreateComponent(ComponentType::CAMERA));

	if (App != nullptr)
	{
		float w = EngineApp->GetWindow()->GetWidth();
		float h = EngineApp->GetWindow()->GetHeight();

		mEditorCamera->SetAspectRatio(w / h);
	}
	mCurrentCamera = mEditorCamera;



	return true;
}

void ModuleEngineCamera::SetCurrentCamera(GameObject* camera)
{
	if (camera)
	{
		mCurrentCamera = reinterpret_cast<CameraComponent*>(camera->GetComponent(ComponentType::CAMERA));
		mGameCamera = mCurrentCamera;
		mIsEditorCameraActive = false;
	}
	else 
	{
		mCurrentCamera = mEditorCamera;
		mGameCamera = nullptr;
		mIsEditorCameraActive = true;
	}
}

void ModuleEngineCamera::SetCurrentCamera(CameraComponent* camera)
{
	if (camera)
	{
		mCurrentCamera = camera;
		mGameCamera = mCurrentCamera;
		mIsEditorCameraActive = false;
	}
	else
	{
		mCurrentCamera = mEditorCamera;
		mGameCamera = nullptr;
		mIsEditorCameraActive = true;
	}
}

update_status ModuleEngineCamera::Update(float dt)
{

	//TODO: SEPARATE GAME ENGINE
	//if (mDrawRayCast) 
	//{
	//	App->GetDebugDraw()->DrawLine(mRay.pos, mRay.dir, float3(1.0f, 0.0f, 0.0f));
	//}
	bool hasBeenUpdated = false;

	if (mIsEditorCameraActive && ((ScenePanel*)EngineApp->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
	{
		//#ifdef ENGINE


		const float dtTransformCameraVel = dt * 3.f;
		float transformCameraVel = 0.03f;
		const float rotateCameraVel = 0.01f;

		const float dtFastSpeed = dtTransformCameraVel * mShiftSpeed;
		const float fastSpeed = transformCameraVel * 3.0f;
		bool shiftPressed = (EngineApp->GetInput()->GetKey(KeyboardKeys_LSHIFT) == KeyState::KEY_REPEAT) || (App->GetInput()->GetKey(KeyboardKeys_RSHIFT) == KeyState::KEY_REPEAT);
		float dtSpeed = shiftPressed ? dtFastSpeed : dtTransformCameraVel;
		float speed = shiftPressed ? fastSpeed : transformCameraVel;

		mShiftSpeed = shiftPressed ? mShiftSpeed + 0.05 : 5.0f;


		if (App->GetInput()->GetMouseWheelMotion() != 0)
		{
			mCurrentCamera->Transform(float3(0, 0, speed * 10.f * App->GetInput()->GetMouseWheelMotion()));
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			hasBeenUpdated = true;
			App->GetInput()->GetMouseMotion(mX, mY);
			mCurrentCamera->Rotate(float3::unitY, -mX * rotateCameraVel);
			mCurrentCamera->Rotate(mCurrentCamera->GetFrustum().WorldRight(), -mY * rotateCameraVel);
			if (App->GetInput()->GetKey(KeyboardKeys_Q) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(0, -dtSpeed, 0));
			}
			if (App->GetInput()->GetKey(KeyboardKeys_E) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(0, dtSpeed, 0));
			}

			if (App->GetInput()->GetKey(KeyboardKeys_W) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(0, 0, dtSpeed));
			}
			if (App->GetInput()->GetKey(KeyboardKeys_S) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(0, 0, -dtSpeed));
			}
			if (App->GetInput()->GetKey(KeyboardKeys_A) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(-dtSpeed, 0, 0));
			}
			if (App->GetInput()->GetKey(KeyboardKeys_D) == KeyState::KEY_REPEAT)
			{
				mCurrentCamera->Transform(float3(dtSpeed, 0, 0));
			}
		}
		//paning camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			mCurrentCamera->Transform(float3(-mX * speed, 0, 0));
			mCurrentCamera->Transform(float3(0, mY * speed, 0));
		}
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
		if (App->GetInput()->GetKey(KeyboardKeys_F) == KeyState::KEY_DOWN)
		{
			//TODO: FIX THIS
			//float3 selectedObjectPosition = ((HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject()->GetPosition();
			//float3 initialCameraPosition = mCurrentCamera->GetFrustum().pos;
			//
			//float desiredDistance = 5.0f;
			//
			//float3 finalCameraPosition = selectedObjectPosition - (desiredDistance * (selectedObjectPosition - initialCameraPosition).Normalized());
			//
			//mCurrentCamera->SetPos(finalCameraPosition);
			//
			//mCurrentCamera->LookAt(mCurrentCamera->GetFrustum().pos, selectedObjectPosition, float3::unitY);
		}


		if (mCurrentCamera && mCurrentCamera && hasBeenUpdated)
		{ //TODO: add a bool if the camera had an input
			float3 position = mCurrentCamera->GetFrustum().pos;
			float3x3 rotationMatrix = float3x3(mCurrentCamera->GetFrustum().WorldRight(), mCurrentCamera->GetFrustum().up, mCurrentCamera->GetFrustum().front);
			Quat rotation = Quat(rotationMatrix);

			//mCurrentCamera->SetPosition(position);
			//mCurrentCamera->SetRotation(rotation);

		}

		//#else
		//TODO: remove the editor camera in game
		if (mCurrentCamera == mEditorCamera)
		{
			const float dtTransformCameraVel = dt * 3.f;
			float transformCameraVel = 0.03f;
			const float rotateCameraVel = 0.01f;

			const float dtFastSpeed = dtTransformCameraVel * mShiftSpeed;
			const float fastSpeed = transformCameraVel * 3.0f;
			bool shiftPressed = (App->GetInput()->GetKey(KeyboardKeys_LSHIFT) == KeyState::KEY_REPEAT) || (App->GetInput()->GetKey(KeyboardKeys_RSHIFT) == KeyState::KEY_REPEAT);
			float dtSpeed = shiftPressed ? dtFastSpeed : dtTransformCameraVel;
			float speed = shiftPressed ? fastSpeed : transformCameraVel;

			mShiftSpeed = shiftPressed ? mShiftSpeed + 0.05 : 5.0f;


			if (App->GetInput()->GetMouseWheelMotion() != 0)
			{
				mCurrentCamera->Transform(float3(0, 0, speed * 10.f * App->GetInput()->GetMouseWheelMotion()));
			}
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
			{
				int mX, mY;
				hasBeenUpdated = true;
				App->GetInput()->GetMouseMotion(mX, mY);
				mCurrentCamera->Rotate(float3::unitY, -mX * rotateCameraVel);
				mCurrentCamera->Rotate(mCurrentCamera->GetFrustum().WorldRight(), -mY * rotateCameraVel);
				if (App->GetInput()->GetKey(KeyboardKeys_Q) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(0, -dtSpeed, 0));
				}
				if (App->GetInput()->GetKey(KeyboardKeys_E) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(0, dtSpeed, 0));
				}

				if (App->GetInput()->GetKey(KeyboardKeys_W) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(0, 0, dtSpeed));
				}
				if (App->GetInput()->GetKey(KeyboardKeys_S) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(0, 0, -dtSpeed));
				}
				if (App->GetInput()->GetKey(KeyboardKeys_A) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(-dtSpeed, 0, 0));
				}
				if (App->GetInput()->GetKey(KeyboardKeys_D) == KeyState::KEY_REPEAT)
				{
					mCurrentCamera->Transform(float3(dtSpeed, 0, 0));
				}
			}
			//paning camera
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
			{
				int mX, mY;
				App->GetInput()->GetMouseMotion(mX, mY);
				mCurrentCamera->Transform(float3(-mX * speed, 0, 0));
				mCurrentCamera->Transform(float3(0, mY * speed, 0));
			}
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
			}
		}
		if (mCurrentCamera && mCurrentCamera && hasBeenUpdated)
		{ //TODO: add a bool if the camera had an input
			float3 position = mCurrentCamera->GetFrustum().pos;
			float3x3 rotationMatrix = float3x3(mCurrentCamera->GetFrustum().WorldRight(), mCurrentCamera->GetFrustum().up, mCurrentCamera->GetFrustum().front);
			Quat rotation = Quat(rotationMatrix);

			//mCurrentCamera->SetPosition(position);
			//mCurrentCamera->SetRotation(rotation);
		}
	}

	//#endif // ENGINE
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
	if (mCurrentCamera->GetID() != mEditorCamera->GetID())
	{
		mGameCamera = mCurrentCamera;
		mCurrentCamera = mEditorCamera;
		mIsEditorCameraActive = true;
	}
}

void ModuleEngineCamera::ActivateGameCamera()
{

	if (mCurrentCamera->GetID() == mEditorCamera->GetID())
	{
		if (mGameCamera)
		{
			mCurrentCamera = mGameCamera;
			mIsEditorCameraActive = false;
		}
	}
}

void ModuleEngineCamera::SetEditorCameraPosition(float3 position)
{
	mEditorCamera->SetPos(position);
}

void ModuleEngineCamera::SetEditorCameraFrontUp(float3 front, float3 up)
{
	mEditorCamera->SetFrontUp(front, up);
}

