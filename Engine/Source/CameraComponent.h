#pragma once
#include "Component.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

class ENGINE_API CameraComponent :
    public Component
{
public:
    CameraComponent(GameObject* owner);
    CameraComponent(const CameraComponent& original, GameObject* owner);
    ~CameraComponent();

    void Update() override;
    Component* Clone(GameObject* owner) const override;
    void Reset() override;
    void SetPosition(const float3& position);
    void SetRotation(const float3& rotation);

    void SetNearPlane(const float value) { mCamera->nearPlaneDistance = value;};
    void SetFarPlane(const float value) { mCamera->farPlaneDistance = value; };
    void SetVerticicalFOV(const float value) { mCamera->verticalFov = value; };

    float GetNearPlane() { return mCamera->nearPlaneDistance; };
    float GetFarPlane() { return mCamera->farPlaneDistance; };
    float GetVerticicalFOV() { return mCamera->verticalFov; };

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    Frustum* mCamera;
    bool mEnableCulling;
};

