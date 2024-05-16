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
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	void SetMesh(unsigned int uid);
	const AABB& GetAABB() const { return mAABB; }
	const AABB& GetAABBWorld() const { return mAABBWorld; }

	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMaterial(unsigned int uid);

private:
	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;
	int mTemporalID = -1;
};
