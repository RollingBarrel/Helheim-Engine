#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Mesh;
class Material;
class ResourceMesh;
class ResourceMaterial;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner, unsigned int meshUid, unsigned int materialUid);
	MeshRendererComponent(const MeshRendererComponent& other, GameObject* owner);
	~MeshRendererComponent();

	void Draw();
	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void RefreshBoundingBoxes();
	const OBB getOBB() const { return mOBB; }
	const AABB GetAABB() const { return mAABB; }
	const AABB GetAABBWorld() const { return mAABBWorld; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }

	const ResourceMaterial* GetMaterial() const { return mMaterial; }
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
private:
	ResourceMesh* mMesh;
	ResourceMaterial* mMaterial;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	AABB mAABBWorld;
	bool mDrawBox = false;
	bool mInsideFrustum = true;

	int mTemporalID = -1;
};
