#include "ModuleCamera.h"
#include "Math/MathConstants.h"
#include "Geometry/LineSegment.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"
#include "ModuleOpenGL.h"
#include "HierarchyPanel.h"
#include "ModuleDebugDraw.h"
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "imgui.h"
#include "Geometry/Triangle.h"
#include "Quadtree.h"
#include "SDL_scancode.h"
#include "GameObject.h"
#include "Quat.h"
#include "CameraComponent.h"
#include <map>


bool ModuleCamera::Init()
{
	CreateEditorCamera();

	return true;
}

void ModuleCamera::CheckRaycast()
{

	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	int mouseAbsoluteX = scenePanel->GetMousePosition().x;
	int mouseAbsoluteY = scenePanel->GetMousePosition().y;

	float normalizedX = -1.0 + 2.0 * (float)(mouseAbsoluteX - scenePanel->GetWindowsPos().x) / (float)scenePanel->GetWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mouseAbsoluteY - scenePanel->GetWindowsPos().y) / (float)scenePanel->GetWindowsSize().y;

	LineSegment raySegment = mCurrentCameraComponent->GetFrustum().UnProjectLineSegment(normalizedX, normalizedY);

	mRay.pos = raySegment.a;
	mRay.dir = (raySegment.b - raySegment.a);

	bool intersects = false;
	bool intersectsTriangle = false;

	Quadtree* root = App->GetScene()->GetQuadtreeRoot();

	if (reinterpret_cast<ScenePanel*>(App->GetEditor()->GetPanel(SCENEPANEL))->IsGuizmoUsing()) 
	{

	}
	else 
	{

		std::map<float, GameObject*> hits = root->RayCast(&mRay);
		if (!hits.empty()) 
		{
			const std::pair<float, GameObject*> intersectGameObjectPair = std::pair<float, GameObject*>(hits.begin()->first, hits.begin()->second);
			if (intersectGameObjectPair.second != nullptr)
			{
				GameObject* gameObject = intersectGameObjectPair.second;
				while (!gameObject->GetParent()->IsRoot())
				{
					gameObject = gameObject->GetParent();
				}
				((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(gameObject);
			}
		}
	}
}



bool ModuleCamera::CleanUp()
{
	delete mEditorCamera;
	return true;
}

const void ModuleCamera::CreateEditorCamera()
{
	mEditorCamera = new GameObject("EditorCamera", nullptr);

	mEditorCamera->CreateComponent(ComponentType::CAMERA);

	CameraComponent* camera = reinterpret_cast<CameraComponent*>(mEditorCamera->GetComponent(ComponentType::CAMERA));

	if (App != nullptr)
	{
		float w = App->GetWindow()->GetWidth();
		float h = App->GetWindow()->GetHeight();

		camera->SetAspectRatio(w / h);
	}

	mCurrentCameraComponent = camera;

	mCurrentCamera = mEditorCamera;
}

void ModuleCamera::SetCurrentCamera(GameObject* camera)
{
	if (camera->GetComponent(ComponentType::CAMERA))
	{
		mCurrentCamera = camera;
		mCurrentCameraComponent = (CameraComponent*) camera->GetComponent(ComponentType::CAMERA);
		App->GetOpenGL()->SetOpenGlCameraUniforms();

	}
	if (!camera)
		mCurrentCamera = mEditorCamera;
}

const CameraComponent* ModuleCamera::GetCurrentCamera() const
{
	return (CameraComponent*)mCurrentCamera->GetComponent(ComponentType::CAMERA);
}

const CameraComponent* ModuleCamera::GetEditorCamera() const
{
	return (CameraComponent*)mEditorCamera->GetComponent(ComponentType::CAMERA);
}

const float3& ModuleCamera::GetPosition() const
{
	return mEditorCamera->GetPosition();
}

const float3& ModuleCamera::GetRotation() const
{
	return mEditorCamera->GetRotation();
}

void ModuleCamera::SetPosition(float3 newPostion)
{
	mEditorCamera->SetPosition(newPostion);
	mCurrentCameraComponent->SetPos(newPostion);
}

void ModuleCamera::SetRotation(float3 newRotation)
{
	mCurrentCamera->SetRotation(newRotation);
	mCurrentCameraComponent->UpdateRotation();
}

void ModuleCamera::SetFrontUp(float3 front, float3 up) 
{ 
	mCurrentCameraComponent->SetFrontUp(front, up); 
	//float3x3 rotationMatrix = float3x3(Cross(front, up), up, front);
	//Quat rotation = Quat(rotationMatrix);
	//mEditorCamera->SetRotation(rotation);

	//mEditorCamera->Update();
}

void ModuleCamera::ActivateEditorCamera()
{
	SetCurrentCamera(mEditorCamera);
}

update_status ModuleCamera::Update(float dt)
{


	if (mDrawRayCast) 
	{
		App->GetDebugDraw()->DrawLine(mRay.pos, mRay.dir, float3(1.0f, 0.0f, 0.0f));
	}
	
	const auto& io = ImGui::GetIO();
	bool hasBeenUpdated = false;

	if (((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
	{
		if (mCurrentCamera == mEditorCamera)
		{
			const float dtTransformCameraVel = dt * 3.f;
			float transformCameraVel = 0.03f;
			const float rotateCameraVel = 0.01f;
		
			const float dtFastSpeed = dtTransformCameraVel * 3.0f;
			const float fastSpeed = transformCameraVel * 3.0f;
			bool shiftPressed = (App->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) || (App->GetInput()->GetKey(SDL_SCANCODE_RSHIFT) == KeyState::KEY_REPEAT);
			float dtSpeed = shiftPressed ? dtFastSpeed : dtTransformCameraVel;
			float speed = shiftPressed ? fastSpeed : transformCameraVel;

	
		
			if (App->GetInput()->GetMouseWheelMotion() != 0)
			{
				mCurrentCameraComponent->Transform(float3(0, 0, speed * 10.f * App->GetInput()->GetMouseWheelMotion()));
			}
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
			{
				int mX, mY;
				hasBeenUpdated = true;
				App->GetInput()->GetMouseMotion(mX, mY);
				mCurrentCameraComponent->Rotate(float3::unitY, -mX * rotateCameraVel);
				mCurrentCameraComponent->Rotate(mCurrentCameraComponent->GetFrustum().WorldRight(), -mY * rotateCameraVel);
			if (App->GetInput()->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
			{
				mCurrentCameraComponent->Transform(float3(0, -dtSpeed, 0));
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
			{
				mCurrentCameraComponent->Transform(float3(0, dtSpeed, 0));
			}
		
				if (App->GetInput()->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
				{
					mCurrentCameraComponent->Transform(float3(0, 0, dtSpeed));
				}
				if (App->GetInput()->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
				{
					mCurrentCameraComponent->Transform(float3(0, 0, -dtSpeed));
				}
				if (App->GetInput()->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
				{
					mCurrentCameraComponent->Transform(float3(-dtSpeed, 0, 0));
				}
				if (App->GetInput()->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
				{
					mCurrentCameraComponent->Transform(float3(dtSpeed, 0, 0));
				}
			}
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
			{
				CheckRaycast();
			}
			//paning camera
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
			{
				int mX, mY;
				App->GetInput()->GetMouseMotion(mX, mY);
				mCurrentCameraComponent->Transform(float3(-mX * speed, 0, 0));
				mCurrentCameraComponent->Transform(float3(0, mY * speed, 0));
			}
			//orbiting camera
			if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
			{
				float3 focus = mCurrentCameraComponent->GetFrustum().pos;
				int mX, mY;
				App->GetInput()->GetMouseMotion(mX, mY);

				if (mX != 0)
				{
					float3x3 rotationMatrixX = float3x3::RotateAxisAngle(mCurrentCameraComponent->GetFrustum().up,-mX * rotateCameraVel);
					focus = rotationMatrixX.Mul(focus);
				}
				if (mY != 0)
				{
					float3x3 rotationMatrixY = float3x3::RotateAxisAngle(mCurrentCameraComponent->GetFrustum().WorldRight(), mY * rotateCameraVel);
					focus = rotationMatrixY.Mul(focus);
				}

				GameObject* focusedObject = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject();
				if(focusedObject)
					mCurrentCameraComponent->LookAt(focus, focusedObject->GetPosition(), float3::unitY);
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
			{
				float3 selectedObjectPosition = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject()->GetPosition();
				float3 initialCameraPosition = mCurrentCameraComponent->GetFrustum().pos;
		
				float desiredDistance = 5.0f;
		
				float3 finalCameraPosition = selectedObjectPosition - (desiredDistance * (selectedObjectPosition - initialCameraPosition).Normalized());
		
				mCurrentCameraComponent->SetPos(finalCameraPosition);
		
				mCurrentCameraComponent->LookAt(mCurrentCameraComponent->GetFrustum().pos, selectedObjectPosition, float3::unitY);
			}
		}

		if(mCurrentCamera && mCurrentCameraComponent && hasBeenUpdated){ //TODO: add a bool if the camera had an input
			float3 position = mCurrentCameraComponent->GetFrustum().pos;
			float3x3 rotationMatrix = float3x3(mCurrentCameraComponent->GetFrustum().WorldRight(), mCurrentCameraComponent->GetFrustum().up, mCurrentCameraComponent->GetFrustum().front);
			Quat rotation = Quat(rotationMatrix);

			mCurrentCamera->SetPosition(position);
			mCurrentCamera->SetRotation(rotation);

		}
	}
	return UPDATE_CONTINUE;
}
