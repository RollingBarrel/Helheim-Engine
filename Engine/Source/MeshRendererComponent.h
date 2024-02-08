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
	~MeshRendererComponent() { delete mMesh; delete mMaterial; };
	void Clear() {};

	void Draw();
	void Load(unsigned int meshUid, unsigned int materialUid);
	void LoadPBR(const char* uid) {};
	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	const OBB getOBB() const { return mOBB; }
	 ResourceMesh* GetResourceMesh() const { return mMesh; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }

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
