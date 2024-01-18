#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "GameObject.h"
#include "Mesh.h"


class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original , GameObject* owner);
	//~MeshRendererComponent();
	void Clear();
	void Draw();
	void Reset() override;
	void Load(const char* assetFileName);

	void Update() override;
	void DrawEditor();
	Component* Clone(GameObject* owner) override;

	const OBB getOBB() const { return mOBB; }

	void SetInsideFrustum(bool inside) { mInsideFrustum = inside; }
	bool* getShouldDraw() { return mDrawBox; }

private:
	std::vector<Mesh*> mMeshes; //it was a mesh pointer (Mesh*) 
	OBB mOBB;
	bool* mDrawBox = new bool(false);
	bool mInsideFrustum = true;
};

