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
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner);
	~MeshRendererComponent() { delete mMesh; delete mMaterial; };
	void Clear();
	void LoadByIDTemporal(const int id);

	void Draw();
	void Load(unsigned int meshUid, unsigned int materialUid);
	void LoadPBR(const char* uid) {};
	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	const OBB getOBB() const { return mOBB; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }

	const ResourceMaterial* GetMaterial() const { return mMaterial; }

	//UNIFORMS
	float lightDir[3] = { 0.0f, -1.0f, 1.0f };
	float lightColor[3] = { 0.0f, 0.0f, 0.7f };
	float lightIntensity = 10.0f;

	float ambientColor[3] = {0.5f, 0.5f, 0.5f};

private:
	ResourceMesh* mMesh;
	ResourceMaterial* mMaterial;
	void Save(Archive& archive) const override;
	void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

	OBB mOBB;
	AABB mAABB;
	bool mDrawBox = false;
	bool mInsideFrustum = true;

	int mTemporalID = -1;
};
