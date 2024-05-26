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
	void Enable() override;
	void Disable() override;

	void RefreshBoundingBoxes();
	const OBB& GetOBB() const { return mOBB; }
	const AABB& GetAABB() const { return mAABB; }
	const ResourceMesh* GetResourceMesh() const { return mMesh; }
	void SetMesh(unsigned int uid);
	
	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool IsInsideFrustum() const { return mInsideFrustum; }

	const ResourceMaterial* GetResourceMaterial() const { return mMaterial; }
	void SetMaterial(unsigned int uid);

private:
	void Save(JsonObject& obj) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	ResourceMesh* mMesh = nullptr;
	ResourceMaterial* mMaterial = nullptr;

	OBB mOBB;
	AABB mAABB;
	bool mInsideFrustum = true;
	int mTemporalID = -1;
};
