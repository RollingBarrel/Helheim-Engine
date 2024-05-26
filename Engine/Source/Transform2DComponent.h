#pragma once
#include "Component.h"
#include "Math/float2.h" 
#include "Math/Quat.h"
#include "Math/float4x4.h" 

class ComponentCanvas;

class ENGINE_API Transform2DComponent :
    public Component
{
public:
	Transform2DComponent(const bool active, GameObject* owner);
	Transform2DComponent(GameObject* owner);
	Transform2DComponent(const Transform2DComponent& original, GameObject* owner);
	~Transform2DComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;
	void Reset();
	void Save(JsonObject& obj) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	inline const float4x4& GetGlobalMatrix() const { return mGlobalMatrix; }
	inline const float3& GetPosition() const { return mPosition; }
	inline const float3& GetRotation() const { return mEulerRotation; }
	inline const float2& GetSize() const { return mSize; }
	inline const float2& GetAnchorMin() const { return mAnchorMin; }
	inline const float2& GetAnchorMax() const { return mAnchorMax; }
	inline const float2& GetPivot() const { return mPivot; } 

	void SetPosition(const float3& position);
	void SetRotation(const float3& rotation);
	void SetSize(const float2 size);
	void SetAnchorMin(const float2 anchorMin);
	void SetAnchorMax(const float2 anchorMax);
	void SetPivot(const float2 pivot);

	void CalculateMatrices();
	void ResetTransform();

private:
	float3 GetPositionRelativeToParent();
	float3 GetScreenPosition();

	float3 mPosition= float3::zero;
	float3 mEulerRotation = float3::zero;
	Quat mRotation = Quat::identity;
	float2 mSize = float2::one;

	float2 mAnchorMin = float2(0.5, 0.5);
	float2 mAnchorMax = float2(0.5, 0.5);
	float2 mPivot = float2(0,0);

	float4x4 mLocalMatrix = float4x4::identity;
	float4x4 mGlobalMatrix = float4x4::identity;
};

