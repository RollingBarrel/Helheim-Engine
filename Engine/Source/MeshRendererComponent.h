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
	const AABB& GetAABB() const { return mAABB; }
	const AABB& GetAABBWorld() const { return mAABBWorld; }

	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMesh(unsigned int uid);
	void SetMaterial(unsigned int uid);

private:
	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data) override;

	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	OBB mOBB;
	AABB mAABB;
	AABB mOriginalAABB;
	AABB mAABBWorld;
	int mTemporalID = -1;
};
