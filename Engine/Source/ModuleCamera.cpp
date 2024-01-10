#include "ModuleCamera.h"
#include "Math/MathConstants.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "glew.h"

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
	glGenBuffers(1, &cameraUnis);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUnis);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 16 * 2, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUnis);
	//glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, sizeof(float)*16*2);
	return true;
}

void ModuleCamera::WindowResized(int w, int h)
{
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (float)w / (float)h);
}

update_status ModuleCamera::Update()
{
	//Fer state machine amb els inputs !!!!
	//TODO: Camera velocity variable independent of framerate
	const float dtTransformCameraVel = App->GetDt() * 8.f;
	const float dtRotateCameraVel = App->GetDt() * 1.f;

	//moving/rot camera
	if (App->GetInput()->GetMouseWheelMotion() != 0)
	{
		Transform(float3(0, 0, dtTransformCameraVel*10.f * App->GetInput()->GetMouseWheelMotion()));
	}
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		int mX, mY;
		App->GetInput()->GetMouseMotion(mX, mY);
		Rotate(float3::unitY, -mX * dtRotateCameraVel);
		//TODO: save the right vector myself??
		Rotate(frustum.WorldRight(), -mY * dtRotateCameraVel);
		if (App->GetInput()->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT)
		{
			Transform(float3(0, -dtTransformCameraVel, 0));
		}
		if (App->GetInput()->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT)
		{
			Transform(float3(0, dtTransformCameraVel, 0));
		}
		if (App->GetInput()->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT)
		{
			Transform(float3(0, 0, dtTransformCameraVel));
		}
		if (App->GetInput()->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT)
		{
			Transform(float3(0, 0, -dtTransformCameraVel));
		}
		if (App->GetInput()->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT)
		{
			Transform(float3(-dtTransformCameraVel, 0, 0));
		}
		if (App->GetInput()->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT)
		{
			Transform(float3(dtTransformCameraVel, 0, 0));
		}
	}
	//paning camera
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_MIDDLE) == KeyState::KEY_REPEAT)
	{
		int mX, mY;
		App->GetInput()->GetMouseMotion(mX, mY);
		Transform(float3(-mX * dtTransformCameraVel, 0, 0));
		Transform(float3(0, mY * dtTransformCameraVel, 0));
	}
	//orbiting camera
	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_REPEAT && App->GetInput()->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
	{
		float3 right = frustum.WorldRight();
		float3 focus = frustum.pos; //(cameraPos - targetPos)
		int mX, mY;
		App->GetInput()->GetMouseMotion(mX, mY);
		float3x3 rotationMatrix = float3x3::RotateAxisAngle(float3::unitY, -mX * dtRotateCameraVel) * float3x3::RotateAxisAngle(right, -mY * dtRotateCameraVel);
		focus = rotationMatrix.MulDir(focus);
		float3 newUp = rotationMatrix.MulDir(frustum.up);
		LookAt(focus, float3(0, 0, 0), newUp);
	}

	//TODO: aixo nomes si canvia la camera
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUnis);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, GetViewMatrix().Transposed().ptr());
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 16, sizeof(float) * 16, GetProjectionMatrix().Transposed().ptr());

	return UPDATE_CONTINUE;
}

void ModuleCamera::Rotate(const float3& axis, float angleRad)
{
	float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, angleRad);
	frustum.up = rotationMatrix.Mul(frustum.up);
	frustum.front = rotationMatrix.Mul(frustum.front);
}

void ModuleCamera::Transform(float3 vec)
{
	vec.z = -vec.z;
	float3x4 world = frustum.WorldMatrix();
	float3 newTrans = world.TransformDir(vec);
	world.SetTranslatePart(world.TranslatePart() + newTrans);
	frustum.SetWorldMatrix(world);
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
}

bool ModuleCamera::CleanUp()
{
	glDeleteBuffers(1, &cameraUnis);
	return true;
}