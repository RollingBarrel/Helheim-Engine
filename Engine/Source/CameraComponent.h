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
    void Reset() override {}

private:
    void LookAt(float3 eyePos, float3 targetPos, float3 upVector);
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    Frustum mCamera;
};

