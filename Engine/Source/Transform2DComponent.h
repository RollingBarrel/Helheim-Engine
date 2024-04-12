#pragma once
#include "Component.h"
#include "Math/float2.h" 

class ComponentCanvas;

class Transform2DComponent :
    public Component
{
public:
	Transform2DComponent(const bool active, GameObject* owner);
	~Transform2DComponent();

	void Update() override;

	inline const float4x4& GetGlobalMatrix() const { return globalMatrix; }
	inline const float3& GetGlobalPosition() const { return globalMatrix.TranslatePart(); }
	inline const float4x4& GetGlobalRotation() const { static_cast<float4x4>(globalMatrix.RotatePart()); }
	inline const float3& GetGlobalScale() const { return globalMatrix.GetScale(); }

	inline const float3& GetPosition() const { return position; }
	inline const float4x4& GetRotation() const { return rotation; }
	inline const float3& GetScale() const { return scale; }
	inline const float2& GetSize() const { return size; }

	inline void SetPosition(const float3& position) { this->position = position; }
	inline void SetRotation(const float4x4& rotation) { this->rotation = rotation; }
	inline void SetScale(const float3& scale) { this->scale = scale; }
	inline void SetSize(const float2 size) { this->size = size; }

	void CalculateMatrices();

private:
	float3 GetPositionRelativeToParent();
	float3 GetScreenPosition();

	float3 position;
	float4x4 rotation;
	float3 scale;
	float2 size;

	float2 anchorMin = float2(0.5, 0.5);
	float2 anchorMax = float2(0.5, 0.5);
	float2 pivot;

	float4x4 localMatrix;
	float4x4 globalMatrix;
};

