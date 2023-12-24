#include "ModuleCamera.h"
#include "MathGeoLib.h"
#include "SDL.h"
#include "glew.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}




bool ModuleCamera::Init()
{

    frustum.type = FrustumType::PerspectiveFrustum;
    frustum.pos = float3(0.0f, 1.0f, 2.0f);
    frustum.nearPlaneDistance = 0.1f;
    frustum.farPlaneDistance = 200.0f;
    frustum.horizontalFov = DegToRad(90.0f);
    frustum.verticalFov = 2.f * Atan(Tan(frustum.horizontalFov * 0.5f) / 1.3f);;

    frustum.front = float3(1, 0, 0);
    frustum.up = float3(0, 1, 0);
    int x, y;
    SDL_GetMouseState(&x, &y);
    mousePos = float2((float)x, (float)y);


    return true;
}

update_status ModuleCamera::PreUpdate()
{

    return UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
    //TODO: Create the input triggers
    ProcessInput();
    return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
    return UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
    return true;
}

void ModuleCamera::move(const float3& delta)
{
    frustum.pos = frustum.pos - delta;

}

void ModuleCamera::moveForward(bool backwards)
{
    if (cameraType == fixed) {

        int sign = backwards ? 1 : -1;
        move(sign * frustum.front.Normalized() * 0.1f);
    }
}

void ModuleCamera::rotate(float angle, const float3& axis)
{
    float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, angle);
    frustum.front = (rotationMatrix * frustum.front).Normalized();
    frustum.up = (rotationMatrix * frustum.up).Normalized();


}


void ModuleCamera::ProcessInput(const Uint8* keyboard, const int x, const int y)
{

    float speed = 0.01f;
    float2 mouse_delta = float2(mousePos.x - x, mousePos.y - y);
    mousePos = float2((float)x, (float)y);

    if (keyboard[SDL_SCANCODE_LSHIFT])
    {
        speed = speed * 2;
    }
    if (cameraType == movable) {

        if (keyboard[SDL_SCANCODE_W])
        {
            move(-frustum.front.Normalized() * speed);
        }

        if (keyboard[SDL_SCANCODE_S])
        {
            move(frustum.front.Normalized() * speed);

        }

        if (keyboard[SDL_SCANCODE_A])
        {
            move(frustum.WorldRight().Normalized() * speed);

        }

        if (keyboard[SDL_SCANCODE_D])
        {
            move(-frustum.WorldRight().Normalized() * speed);
        }

        if (keyboard[SDL_SCANCODE_Q])
        {
            move(frustum.up.Normalized() * speed);

        }
        if (keyboard[SDL_SCANCODE_E])
        {
            move(-frustum.up.Normalized() * speed);

        }

        if (mouse_delta.x != 0) {
            rotate(mouse_delta.x * 0.01, frustum.up);
        }
        if (mouse_delta.y != 0) {
            rotate(mouse_delta.y * 0.01, frustum.WorldRight());
        }


    }
    else if (cameraType == fixed) {

        if (keyboard[SDL_SCANCODE_UP])
        {
            rotate(speed, frustum.WorldRight());

        }
        if (keyboard[SDL_SCANCODE_DOWN])
        {
            rotate(-speed, frustum.WorldRight());

        }
        if (keyboard[SDL_SCANCODE_LEFT])
        {
            rotate(speed, frustum.up);

        }
        if (keyboard[SDL_SCANCODE_RIGHT])
        {
            rotate(-speed, frustum.up);

        }


    }
    else if (cameraType == CameraType::orbit) {
        //TODO: Change the position of the camera so that the distance remains constant.
        float distanceCameraObject = (float3::zero - frustum.pos).Length();
        if (mouse_delta.x != 0) {
            move(-mouse_delta.x * speed * distanceCameraObject * frustum.WorldRight());
        }
        if (mouse_delta.y != 0) {
            move(mouse_delta.y * speed * distanceCameraObject * frustum.up);
        }
        float3 oldRight = frustum.WorldRight().Normalized();
        frustum.front = (float3::zero - frustum.pos).Normalized();
        frustum.up = Cross(oldRight, frustum.front).Normalized();

        float newDistance = (float3::zero - frustum.pos).Length();
        move(-frustum.front.Normalized() * (newDistance - distanceCameraObject)); //Make sure you don't orbit away


    }

    if (keyboard[SDL_SCANCODE_F])
    {
        //Make this a lookat function Change float3::zero for a float3 input vector
        float3 oldRight = frustum.WorldRight().Normalized();
        frustum.front = (float3::zero - frustum.pos).Normalized();
        frustum.up = Cross(oldRight, frustum.front).Normalized();

    }





}

void ModuleCamera::UpdateProjectionMatrix(int screenWidth, int screenHeight)
{
    // Recalculate the projection matrix with the new aspect ratio
    float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    frustum.horizontalFov = DegToRad(90.0f);
    frustum.verticalFov = 2.0f * atan(tan(frustum.horizontalFov * 0.5f) / aspectRatio);



}

void ModuleCamera::DisplaceFromBoundingBox(float3 mins, float3 maxs)
{
    if (mins.Equals(float3(inf)) || maxs.Equals(float3(-inf))) {
        LOG("Bounding box from model not loaded correctly");
        return;
    }


    float newX = (mins.x - maxs.x) * 2 + (mins.x + maxs.x) / 2;
    float newY = (mins.y - maxs.y) + (mins.y + maxs.y) / 2;
    float newZ = (mins.z + maxs.z) / 2;

    frustum.pos = float3(newX, newY, newZ);

    float3 oldRight = frustum.WorldRight().Normalized();
    frustum.front = (float3::zero - frustum.pos).Normalized();
    frustum.up = Cross(oldRight, frustum.front).Normalized();


}
