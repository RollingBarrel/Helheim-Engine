#pragma once
#include "Component.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Geometry/Frustum.h"

class CameraComponent :
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

    void SetNearPlane(const float value) { mFrustum.nearPlaneDistance = value;};
    void SetFarPlane(const float value) { mFrustum.farPlaneDistance = value; };
    void SetVerticicalFOV(const float value) { mFrustum.verticalFov = value; };

    const float GetNearPlane() { return mFrustum.nearPlaneDistance; };
    const float GetFarPlane() { return mFrustum.farPlaneDistance; };
    const float GetVerticicalFOV() { return mFrustum.verticalFov; };

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    Frustum mFrustum;
    bool mEnableCulling;
};

