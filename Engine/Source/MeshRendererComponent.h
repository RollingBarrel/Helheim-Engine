#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Mesh;
struct Material;
class ResourceMesh;
class ResourceMaterial;
class GeometryBatch;


class ENGINE_API MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner, unsigned int meshUid, unsigned int materialUid);
	MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner);
	~MeshRendererComponent();
	
	void Reset() override {}

	void Update() override {}
	Component* Clone(GameObject* owner) const override;

	void RefreshBoundingBoxes();
	const OBB getOBB() const { return mOBB; }
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	const AABB GetAABB() const { return mAABB; }
	const AABB GetAABBWorld() const { return mAABBWorld; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }
	bool IsInsideFrustum() const { return mInsideFrustum;  }
	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
private:
	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;
	bool mDrawBox = false;
	bool mInsideFrustum = true;
	int mTemporalID = -1;

	
};
