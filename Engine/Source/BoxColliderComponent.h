#pragma once
#include "Component.h"

#include "Math/float3.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class ENGINE_API BoxColliderComponent : public Component
{
public:
	BoxColliderComponent(GameObject* owner);
	~BoxColliderComponent();

	void Reset() override;
	void Update() override {}

	void Draw();

	void OnCollision();
	void ComputeBoundingBox();

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
	AABB mLocalAABB = { float3(-0.5f), float3(0.5f) };
	OBB mWorldOBB = { mLocalAABB };
	float3 mCenter = float3::zero;
	float3 mSize = float3::one;

};

