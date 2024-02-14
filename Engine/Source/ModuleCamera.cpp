#include "ModuleCamera.h"
#include "Math/MathConstants.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ScenePanel.h"
#include "ModuleEditor.h"
#include "ModuleOpenGL.h"
#include "HierarchyPanel.h"

#include "imgui.h"

bool ModuleCamera::Init()
{
	frustum.type = FrustumType::PerspectiveFrustum;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = math::pi / 4.0f;
	int w = App->GetWindow()->GetWidth();
	int h = App->GetWindow()->GetHeight();
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (float)w / (float)h);
	LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY);

	return true;
}

void ModuleCamera::WindowResized(int w, int h)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (float)w / (float)h);
}

update_status ModuleCamera::Update(float dt)
{    

	const auto& io = ImGui::GetIO();
	//ImGuiID id =  ImGui::GetID(SCENEPANEL);

	if (((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->isHovered())
	{
		//Fer state machine amb els inputs !!!!
		//TODO: Camera velocity variable independent of framerate
		const float dtTransformCameraVel = dt * 8.f;							//For some reason the velocities of movement with GetDt() implemented are not consistent
		float transformCameraVel = 0.03f; //Temporary transform velocity (until good GetDt() implementation done)
		const float rotateCameraVel = 0.01f;
		
		//Aply speed when shift
		const float fastSpeed = dtTransformCameraVel * 3.0f;
		bool shiftPressed = (App->GetInput()->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) || (App->GetInput()->GetKey(SDL_SCANCODE_RSHIFT) == KeyState::KEY_REPEAT);
		float speed = shiftPressed ? fastSpeed : dtTransformCameraVel;
		transformCameraVel = shiftPressed ? transformCameraVel * 3.0f : transformCameraVel;

		//moving/rot camera
		if (App->GetInput()->GetMouseWheelMotion() != 0)
		{
			Transform(float3(0, 0, transformCameraVel * 10.f * App->GetInput()->GetMouseWheelMotion()));	//GetDt() should not influence this movement (it depends on the mouse wheel rotation) --- FIXED
		}
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			Rotate(float3::unitY, -mX * rotateCameraVel);									//GetDt() should not influence this rotation (it depends on the mouse position on the screen) --- FIXED
			//TODO: save the right vector myself??
			Rotate(frustum.WorldRight(), -mY * rotateCameraVel);							//GetDt() should not influence this rotation (it depends on the mouse position on the screen) --- FIXED
			if (App->GetInput()->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
			{
				Transform(float3(0, -speed, 0));
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
			{
				Transform(float3(0, speed, 0));
			}

			if (App->GetInput()->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
			{
				Transform(float3(0, 0, speed));
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
			{
				Transform(float3(0, 0, -speed));
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
			{
				Transform(float3(-speed, 0, 0));
			}
			if (App->GetInput()->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
			{
				Transform(float3(speed, 0, 0));
			}
		}
		//paning camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
		{
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			Transform(float3(-mX * transformCameraVel, 0, 0));						//GetDt should influence this movement (it depends on the mouse position on the screen)	--- FIXED
			Transform(float3(0, mY * transformCameraVel, 0));							//GetDt should influence this movement (it depends on the mouse position on the screen) --- FIXED
		}
		//orbiting camera
		if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			float3 right = frustum.WorldRight();
			float3 focus = frustum.pos; //(cameraPos - targetPos)
			int mX, mY;
			App->GetInput()->GetMouseMotion(mX, mY);
			float3x3 rotationMatrix = float3x3::RotateAxisAngle(float3::unitY, -mX * rotateCameraVel) * float3x3::RotateAxisAngle(right, -mY * rotateCameraVel);
			focus = rotationMatrix.MulDir(focus);
			float3 newUp = rotationMatrix.MulDir(frustum.up);
			LookAt(focus, float3(0, 0, 0), newUp);
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

void ModuleCamera::Rotate(const float3& axis, float angleRad)
{
	float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, angleRad);
	frustum.up = rotationMatrix.Mul(frustum.up);
	frustum.front = rotationMatrix.Mul(frustum.front);

	App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void ModuleCamera::Transform(float3 vec)
{
	vec.z = -vec.z;
	float3x4 world = frustum.WorldMatrix();
	float3 newTrans = world.TransformDir(vec);
	world.SetTranslatePart(world.TranslatePart() + newTrans);
	frustum.SetWorldMatrix(world);

	App->GetOpenGL()->SetOpenGlCameraUniforms();
}


void ModuleCamera::LookAt(float3 eyePos, float3 targetPos, float3 upVector) {
	float3 forward = (targetPos - eyePos);
	forward.Normalize();
	float3 right = math::Cross(forward, upVector);
	right.Normalize();
	float3 up = math::Cross(right, forward);
	up.Normalize();

	frustum.pos = eyePos;
	frustum.front = forward;
	frustum.up = up;

	App->GetOpenGL()->SetOpenGlCameraUniforms();
}
