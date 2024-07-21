#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include <vector>
#include <unordered_map>
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
	const AABB& GetOriginalAABB() const { return mOriginalAABB; }
	const AABB& GetAABBWorld() const { return mAABBWorld; }

	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMesh(unsigned int uid);
	void SetMaterial(unsigned int uid);
	void SetInvBindMatrices(std::vector<std::pair<GameObject*, float4x4>>&& bindMatrices, const MeshRendererComponent* palette = nullptr);
	void UpdateSkeletonObjects(const std::unordered_map<const GameObject*, GameObject*>& originalToNew);

	const std::vector<float4x4>& GetPalette() const { return (mPaletteOwner) ? mPaletteOwner->GetPalette() : mPalette; }
	bool HasSkinning() const { return mHasSkinning; };

	void Enable() override;
	void Disable() override;
	

private:
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

	void UpdatePalette();


	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	OBB mLocalOBB;
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
