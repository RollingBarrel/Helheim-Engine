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
#include "CameraUtils.h"
#include "HierarchyPanel.h"
#include "ModuleDebugDraw.h"
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "imgui.h"
#include "Geometry/Triangle.h"


bool ModuleCamera::Init()
{
	mFrustum = CameraUtils::InitiateCamera(float3(0.0f, 4.0f, 8.0f));

	return true;
}

void ModuleCamera::WindowResized(int w, int h)
{
	mFrustum->horizontalFov = 2.f * atanf(tanf(mFrustum->verticalFov * 0.5f) * (float)w / (float)h);
}



void ModuleCamera::CheckRaycast()
{

	ScenePanel* scenePanel = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL));

	int mouseAbsoluteX = scenePanel->GetMousePosition().x;
	int mouseAbsoluteY = scenePanel->GetMousePosition().y;

	float normalizedX = -1.0 + 2.0 * (float)(mouseAbsoluteX - scenePanel->GetWindowsPos().x) / (float)scenePanel->GetWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mouseAbsoluteY - scenePanel->GetWindowsPos().y) / (float)scenePanel->GetWindowsSize().y;

	LineSegment raySegment = frustum.UnProjectLineSegment(normalizedX, normalizedY);

	mRay.pos = raySegment.a;
	mRay.dir = (raySegment.b - raySegment.a);

	bool intersects = false;
	bool intersectsTriangle = false;


	std::map<float, GameObject*> intersectMap;

	// TODO: final implementation when Quadtree is added in this logic
	for (auto gameObject : App->GetScene()->GetRoot()->GetChildren()) {
		// TODO: We need to iterate recursively to all gameobjects in the future. Temporal implementation.
		for (auto child : gameObject->GetChildren()) {
			
			MeshRendererComponent* rMesh = (MeshRendererComponent*)child->GetComponent(ComponentType::MESHRENDERER);

			if (rMesh != nullptr) {

				Ray localRay(mRay);
				localRay.pos = child->GetWorldTransform().Inverted().MulPos(mRay.pos);
				localRay.dir = child->GetWorldTransform().Inverted().MulDir(mRay.dir).Normalized();

				intersects = localRay.Intersects(rMesh->GetAABB());
				if (intersects) 
				{
					unsigned int* indices = rMesh->GetResourceMesh()->mIndices;
					const float* triangles = rMesh->GetResourceMesh()->GetAttributeData(Attribute::POS);

					for (int i = 0; i < rMesh->GetResourceMesh()->mNumIndices / 3; i+=3) {
						float3 verticeA = float3(triangles[indices[i]], triangles[indices[i]+1], triangles[indices[i]+2]);
						float3 verticeB = float3(triangles[indices[i+1]], triangles[indices[i+1] + 1], triangles[indices[i] + 2]);
						float3 verticeC = float3(triangles[indices[i+2]], triangles[indices[i+2] + 1], triangles[indices[i+2] + 2]);

						float distance;
						float3 hitPoint;
						intersectsTriangle = localRay.Intersects(Triangle(verticeA, verticeB, verticeC), &distance, &hitPoint);
						
						if (intersectsTriangle) {
							intersectMap[distance] = child;
							break;
						}
					}
				}
			}
		}
	}

	if (!intersectMap.empty())
	{
		GameObject* gameObject = intersectMap.begin()->second;
		while (!gameObject->GetParent()->IsRoot()) 
		{
			gameObject = gameObject->GetParent();
		}
		((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->SetFocus(gameObject);
	}

}



update_status ModuleCamera::Update(float dt)
{    
	if (mDrawRayCast) {
		App->GetDebugDraw()->DrawLine(mRay.pos, mRay.dir, float3(1.0f, 0.0f, 0.0f));
	}
	
	const auto& io = ImGui::GetIO();


	if (((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
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
			CameraUtils::Transform(float3(0, 0, dtTransformCameraVel*10.f * App->GetInput()->GetMouseWheelMotion()), *mFrustum);
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			CameraUtils::Rotate(float3::unitY, -mX * dtRotateCameraVel,*mFrustum);
			//TODO: save the right vector myself??
			CameraUtils::Rotate(mFrustum->WorldRight(), -mY * dtRotateCameraVel, *mFrustum);
			if (App->GetInput()->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(0, -dtTransformCameraVel, 0), *mFrustum);
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(0, dtTransformCameraVel, 0), *mFrustum);
			}

			if (App->GetInput()->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(0, 0, dtTransformCameraVel), *mFrustum);
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(0, 0, -dtTransformCameraVel), *mFrustum);
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(-dtTransformCameraVel, 0, 0), *mFrustum);
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
			{
				CameraUtils::Transform(float3(dtTransformCameraVel, 0, 0), *mFrustum);
			}
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT)
		{
			CheckRaycast();
		}
		//paning camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			CameraUtils::Transform(float3(-mX * dtTransformCameraVel, 0, 0), *mFrustum);
			CameraUtils::Transform(float3(0, mY * dtTransformCameraVel, 0), *mFrustum);
		}
		//orbiting camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			float3 right = frustum.WorldRight();
			float3 focus = frustum.pos; 
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			float3x3 rotationMatrix = float3x3::RotateAxisAngle(float3::unitY, -mX * rotateCameraVel) * float3x3::RotateAxisAngle(right, -mY * rotateCameraVel);
			focus = rotationMatrix.MulDir(focus);
			float3 newUp = rotationMatrix.MulDir(mFrustum->up);
			CameraUtils::LookAt(focus, float3(0, 0, 0), newUp, *mFrustum);
		}

		if(App->GetInput()->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
		{
			float3 selectedObjectPosition = ((HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL))->GetFocusedObject()->GetLocalPosition();
			float3 initialCameraPosition = frustum.pos;

			float desiredDistance = 5.0f;

			float3 finalCameraPosition = selectedObjectPosition - (desiredDistance * (selectedObjectPosition - initialCameraPosition).Normalized());
			
			frustum.pos = finalCameraPosition;

			LookAt(frustum.pos, selectedObjectPosition, frustum.up);
		}
	}
	return UPDATE_CONTINUE;
}
