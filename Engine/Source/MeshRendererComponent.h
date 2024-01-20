#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Material;
struct ResourceMesh;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original , GameObject* owner);
	~MeshRendererComponent() { delete mMesh; };

	void Draw();
	void Load(const char* uid);
	void Reset() override {}

	void Update() override;
	Component* Clone(GameObject* owner) const override;

	const OBB getOBB() const { return mOBB; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }
	void SetShouldDraw(bool draw) { mDrawBox = draw; }

	void Save(Archive& archive) const override;
	void Load(const rapidjson::Value& data) override;

private:
	ResourceMesh* mMesh;
	Material* material;
	OBB mOBB;
	AABB mAABB;
	bool mDrawBox = false;
	bool mInsideFrustum = true;
};

