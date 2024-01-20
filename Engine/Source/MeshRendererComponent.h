#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
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
	Component* Clone(GameObject* owner) override;

	const OBB getOBB() const { return mOBB; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool ShouldDraw() const { return mDrawBox; }

private:
	ResourceMesh* mMesh;
	Material* material;
	OBB mOBB;
	bool mDrawBox = false;
	bool mInsideFrustum = true;
};

