#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Mesh;
class Material;
class ResourceMesh;
class ResourceMaterial;
class GeometryBatch;


class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner);
	~MeshRendererComponent();
	void Clear() {};

	void Load(unsigned int meshUid, unsigned int materialUid);
	void LoadPBR(const char* uid) {};
	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	const OBB getOBB() const { return mOBB; }
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	const GeometryBatch* GetBatch() const { return mBatch; }
	void SetBatch(GeometryBatch* batch) { mBatch = batch; }
	void CreateCommand();
	const AABB GetAABB() const { return mAABB; }
	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }
	bool IsInsideFrustum() { return mInsideFrustum;  }
	const ResourceMaterial* GetMaterial() const { return mMaterial; }
private:
	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;
	GeometryBatch* mBatch = nullptr;

	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	bool mDrawBox = false;
	bool mInsideFrustum = true;
	int mTemporalID = -1;

	
};
