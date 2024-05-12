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

    void Enable() override;
    void Disable() override;

    void Update() override;
    Component* Clone(GameObject* owner) const override;
    void Reset() override;

    const Frustum& GetFrustum() const { return mFrustum; }

    const float4x4 GetViewMatrix() const { return mFrustum.ViewMatrix(); }
    const float4x4 GetProjectionMatrix() const { return mFrustum.ProjectionMatrix(); }
    const float4x4 GetViewProjectionMatrix() const { return mFrustum.ViewProjMatrix(); }

    void SetNearPlane(float value) { mFrustum.nearPlaneDistance = value;}
    void SetFarPlane(float value) { mFrustum.farPlaneDistance = value; }
    void SetFOV(float value);
    void SetAspectRatio(float value);

    float GetNearPlane() const { return mFrustum.nearPlaneDistance; }
    float GetFarPlane() const { return mFrustum.farPlaneDistance; }
    float GetVerticicalFOV() const { return mFrustum.verticalFov; }


private:

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    Frustum mFrustum;
    bool mEnableCulling = true;

    float mAspectRatio;
};

