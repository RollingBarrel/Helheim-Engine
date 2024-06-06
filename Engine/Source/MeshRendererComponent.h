#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include <vector>
#include "Math/float4x4.h"

class ResourceMesh;
class ResourceMaterial;
class GeometryBatch;
class AnimationComponent;
class ResourceModel;


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
	const OBB& GetOBB() const { return mOBB; }
	const AABB& GetAABB() const { return mAABB; }
	const AABB& GetAABBWorld() const { return mAABBWorld; }

	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMesh(unsigned int uid);
	void SetMaterial(unsigned int uid);
	void SetInvBindMatrices(std::vector<std::pair<GameObject*, float4x4>>&& bindMatrices, const MeshRendererComponent* palette = nullptr);

	const std::vector<float4x4>& GetPalette() const { return (mPaletteOwner) ? mPaletteOwner->GetPalette() : mPalette; }
	bool IsAnimated() const { return mHasSkinning; };


private:
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data) override;

	void UpdatePalette();


	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	OBB mOBB;
	AABB mAABB;
	AABB mOriginalAABB;
	AABB mAABBWorld;
	int mTemporalID = -1;


	//Skinning
	std::vector<std::pair<GameObject*, float4x4>> mGameobjectsInverseMatrices;
	std::vector<float4x4> mPalette;
	const MeshRendererComponent* mPaletteOwner = nullptr;
	bool mHasSkinning = false;

};
