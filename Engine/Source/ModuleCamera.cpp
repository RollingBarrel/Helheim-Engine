#include "ModuleCamera.h"
#include "Math/MathConstants.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"
#include "ModuleOpenGL.h"
#include "CameraUtils.h"
#include "imgui.h"


bool ModuleCamera::Init()
{
	mFrustum = CameraUtils::InitiateCamera();

	return true;
}

void ModuleCamera::WindowResized(int w, int h)
{
	mFrustum->horizontalFov = 2.f * atanf(tanf(mFrustum->verticalFov * 0.5f) * (float)w / (float)h);
}

update_status ModuleCamera::Update()
{    
	const auto& io = ImGui::GetIO();
	//ImGuiID id =  ImGui::GetID(SCENEPANEL);
	if (((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
	{
		//Fer state machine amb els inputs !!!!
		//TODO: Camera velocity variable independent of framerate
		const float dtTransformCameraVel = App->GetDt() * 8.f;
		const float dtRotateCameraVel = App->GetDt() * 1.f;

		//moving/rot camera
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
			float3 right = mFrustum->WorldRight();
			float3 focus = mFrustum->pos; //(cameraPos - targetPos)
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			float3x3 rotationMatrix = float3x3::RotateAxisAngle(float3::unitY, -mX * dtRotateCameraVel) * float3x3::RotateAxisAngle(right, -mY * dtRotateCameraVel);
			focus = rotationMatrix.MulDir(focus);
			float3 newUp = rotationMatrix.MulDir(mFrustum->up);
			CameraUtils::LookAt(focus, float3(0, 0, 0), newUp, *mFrustum);
		}
	}
	return UPDATE_CONTINUE;
}
