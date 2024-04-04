#pragma once
#include "Component.h"
#include "Math/float2.h" 

class ComponentCanvas;

class Transform2DComponent :
    public Component
{
public:
	Transform2DComponent(const bool active, GameObject* owner);
	Transform2DComponent(GameObject* owner);
	~Transform2DComponent();

	void Update() override;
	Component* Clone(GameObject* owner) const override;
	void Reset();
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	inline const float4x4& GetGlobalMatrix() const { return mGlobalMatrix; }
	inline const float3& GetGlobalPosition() const { return mGlobalMatrix.TranslatePart(); }
	inline const float4x4& GetGlobalRotation() const { static_cast<float4x4>(mGlobalMatrix.RotatePart()); }
	inline const float3& GetGlobalScale() const { return mGlobalMatrix.GetScale(); }

	inline const float3& GetPosition() const { return mPosition; }
	inline const float3& GetRotation() const { return mEulerRotation; }
	inline const float2& GetSize() const { return mSize; }

	inline void SetPosition(const float3& position) { this->mPosition = position; }
	inline void SetRotation(const float3& rotation) { this->mRotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z); mEulerRotation = rotation;}
	inline void SetSize(const float2 size) { this->mSize = size; }

	void CalculateMatrices();
	void ResetTransform();

private:
	float3 GetPositionRelativeToParent();
	float3 GetScreenPosition();

	float3 mPosition;
	float3 mEulerRotation;
	Quat mRotation;
	float2 mSize;

	float2 mAnchorMin = float2(0.5, 0.5);
	float2 mAnchorMax = float2(0.5, 0.5);
	float2 mPivot;

	float4x4 mLocalMatrix;
	float4x4 mGlobalMatrix;
};

