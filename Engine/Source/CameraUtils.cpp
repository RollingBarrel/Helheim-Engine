#include "CameraUtils.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "Math/MathConstants.h"
#include "ModuleOpenGL.h"

Frustum* CameraUtils::InitiateCamera(float3 initialPosition)
{
    Frustum* frustum = new Frustum();

    frustum->type = FrustumType::PerspectiveFrustum;
    frustum->nearPlaneDistance = 0.1f;
    frustum->farPlaneDistance = 100.0f;
    frustum->verticalFov = math::pi / 4.0f;

    int w = App->GetWindow()->GetWidth();
    int h = App->GetWindow()->GetHeight();
    frustum->horizontalFov = 2.f * atanf(tanf(frustum->verticalFov * 0.5f) * (float)w / (float)h);

    LookAt(initialPosition, float3(0.0f, 0.0f, -1.0f), float3::unitY, *frustum);

    return frustum;
}

void CameraUtils::LookAt(float3 eyePos, float3 targetPos, float3 upVector, Frustum& camera)
{
    float3 forward = (targetPos - eyePos);
    forward.Normalize();
    float3 right = math::Cross(forward, upVector);
    right.Normalize();
    float3 up = math::Cross(right, forward);
    up.Normalize();

    camera.pos = eyePos;
    camera.front = forward;
    camera.up = up;

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraUtils::Rotate(const float3& axis, float angleRad, Frustum& camera)
{
    float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, angleRad);

    camera.front = rotationMatrix.Mul(camera.front).Normalized();
    camera.up = rotationMatrix.Mul(camera.up).Normalized();

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraUtils::Transform(float3 vec, Frustum& camera)
{
    vec.z = -vec.z;
    float3x4 world = camera.WorldMatrix();
    float3 newTrans = world.TransformDir(vec);
    world.SetTranslatePart(world.TranslatePart() + newTrans);
    camera.SetWorldMatrix(world);

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraUtils::CameraComponentTransform(float3 vec, Frustum& camera)
{
    float3x4 world = camera.WorldMatrix();
    float3 newTrans = world.TransformDir(vec);
    world.SetTranslatePart(world.TranslatePart() + newTrans);
    camera.SetWorldMatrix(world);

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}