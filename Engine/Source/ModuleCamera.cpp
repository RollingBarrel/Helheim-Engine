#include "ModuleCamera.h"
#include "MathGeoLib.h"
#include "SDL.h"
#include "glew.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

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

    int sign = backwards ? 1 : -1;
    move(sign * frustum.front.Normalized() * 0.1f);
    
}

void ModuleCamera::rotate(float angle, const float3& axis)
{
    float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, DegToRad(angle));

    float3 newfront, newup;
    newfront = rotationMatrix.MulDir(frustum.front.Normalized());
    newup = rotationMatrix.MulDir(frustum.up.Normalized());
    frustum.front = newfront.Normalized();
    frustum.up = newup.Normalized();

}


void ModuleCamera::ProcessInput()
{

    float speed = 0.01f;
    int x, y;
    SDL_GetMouseState(&x, &y);

    float2 mouse_delta = float2(mousePos.x - x, mousePos.y - y);
    mousePos = float2((float)x, (float)y);


    
    if (App->GetInput()->GetKey(SDL_SCANCODE_LSHIFT))
    {
        speed = speed * 2;
    }
    if (App->GetInput()->GetMouseButtonDown(SDL_BUTTON_RIGHT)) {

        if (App->GetInput()->GetKey(SDL_SCANCODE_W))
        {
            move(-frustum.front.Normalized() * speed);
        }

        if (App->GetInput()->GetKey(SDL_SCANCODE_S))
        {
            move(frustum.front.Normalized() * speed);

        }

        if (App->GetInput()->GetKey(SDL_SCANCODE_A))
        {
            move(frustum.WorldRight().Normalized() * speed);

        }

        if (App->GetInput()->GetKey(SDL_SCANCODE_D))
        {
            move(-frustum.WorldRight().Normalized() * speed);
        }

        if (App->GetInput()->GetKey(SDL_SCANCODE_Q))
        {
            move(frustum.up.Normalized() * speed);

        }
        if (App->GetInput()->GetKey(SDL_SCANCODE_E))
        {
            move(-frustum.up.Normalized() * speed);

        }

        if (mouse_delta.x != 0) {
            rotate(mouse_delta.x , float3::unitY);
        }
        if (mouse_delta.y != 0) {
            rotate(mouse_delta.y , frustum.WorldRight());
        }


    }
    else if (App->GetInput()->GetMouseButtonDown(SDL_BUTTON_LEFT) && App->GetInput()->GetKey(SDL_SCANCODE_LALT)) {
        

        float distanceCameraObject = (float3::zero - frustum.pos).Length();
        if (mouse_delta.x != 0) {
            move(-mouse_delta.x * speed * 2 * distanceCameraObject * frustum.WorldRight());
        }
        if (mouse_delta.y != 0) {
            move(mouse_delta.y * speed * 2 * distanceCameraObject * frustum.up);
        }
        float3 oldRight = frustum.WorldRight().Normalized();
        frustum.front = (float3::zero - frustum.pos).Normalized();
        frustum.up = Cross(oldRight, frustum.front).Normalized();

        float newDistance = (float3::zero - frustum.pos).Length();
        move(-frustum.front.Normalized() * (newDistance - distanceCameraObject)); //Make sure you don't orbit away


    }
    else {

        if (App->GetInput()->GetKey(SDL_SCANCODE_UP))
        {
            rotate(speed * 100, frustum.WorldRight());

        }
        if (App->GetInput()->GetKey(SDL_SCANCODE_DOWN))
        {
            rotate(-speed * 100, frustum.WorldRight());

        }
        if (App->GetInput()->GetKey(SDL_SCANCODE_LEFT))
        {
            rotate(speed * 100, float3::unitY);

        }
        if (App->GetInput()->GetKey(SDL_SCANCODE_RIGHT))
        {
            rotate(-speed * 100, float3::unitY);

        }


    }


    if (App->GetInput()->GetKey(SDL_SCANCODE_F))
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



