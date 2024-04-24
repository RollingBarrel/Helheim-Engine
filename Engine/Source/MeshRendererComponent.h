#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include <vector>
#include "Math/float4x4.h"

class Mesh;
struct Material;
class ResourceMesh;
class ResourceMaterial;
class GeometryBatch;
class AnimationComponent;


class ENGINE_API MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner);
	~MeshRendererComponent();

	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void RefreshBoundingBoxes();
	const OBB getOBB() const { return mOBB; }
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	void SetMesh(unsigned int uid);
	const AABB GetAABB() const { return mAABB; }
	const AABB GetAABBWorld() const { return mAABBWorld; }

	void Enable() override;
	void Disable() override;

	void LoadAnimatedMesh(bool isAnimated);
	//void MultiplyBindMatrices();
	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	//bool ShouldDraw() const { return mDrawBox; }
	//void SetShouldDraw(bool draw) { mDrawBox = draw; }
	bool IsInsideFrustum() const { return mInsideFrustum; }
	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMaterial(unsigned int uid);
	const std::vector<float4x4> GetPalette() const { return mPalette; }

	bool GetIsAnimated() const { return mIsAnimated; }
	const AnimationComponent* GetAnimationComponent() const { return mAnimationComponent; }
	void SetIsAnimated(bool isAnimated) { mIsAnimated = isAnimated; }
private:
	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;
	//bool mDrawBox = false;
	bool mInsideFrustum = true;
	int mTemporalID = -1;

	std::vector<float4x4> mPalette;
	bool mIsAnimated = false;
	AnimationComponent* mAnimationComponent = nullptr;
};
