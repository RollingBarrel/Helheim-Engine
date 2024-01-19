#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "GameObject.h"

class Material;
struct Mesh;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original , GameObject* owner);
	//~MeshRendererComponent();

	void Draw();
	void Reset() override;
	void Load();

	void Update() override;
	void DrawEditor();
	Component* Clone(GameObject* owner) const override;

	const OBB getOBB() const { return mOBB; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool* getShouldDraw() { return mDrawBox; }

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();
	Mesh* mMesh;
	Material* material;
	OBB mOBB;
	bool* mDrawBox = new bool(false);
	bool mInsideFrustum = true;
};

