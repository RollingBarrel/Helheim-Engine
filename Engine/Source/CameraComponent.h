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

    const Frustum& GetFrustum() { return mFrustum; }

    const float4x4 GetViewMatrix() const { return mFrustum.ViewMatrix(); }
    const float4x4 GetProjectionMatrix() const { return mFrustum.ProjectionMatrix(); }
    const float4x4 GetViewProjectionMatrix() const { return mFrustum.ViewProjMatrix(); }
    void SetPos(const float3& value) { mFrustum.pos = value; }
    void SetNearPlane(const float value) { mFrustum.nearPlaneDistance = value;}
    void SetFarPlane(const float value) { mFrustum.farPlaneDistance = value; }
    void SetFOV(const float value);
    void SetAspectRatio(const float value);

    float GetNearPlane() const { return mFrustum.nearPlaneDistance; };
    float GetFarPlane() const { return mFrustum.farPlaneDistance; };
    float GetVerticicalFOV() const { return mFrustum.verticalFov; };
private:



    void LookAt(float3 eyePos, float3 targetPos, float3 upVector);
    void Rotate(const float3& axis, float angleRad);
    void Transform(float3 vec);
    void CameraComponentTransform(float3 vec);
    void SetRotation(const float3& rotation);


    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    Frustum mFrustum;
    bool mEnableCulling = true;

    float mAspectRatio;
};

