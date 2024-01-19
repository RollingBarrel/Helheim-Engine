#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "GameObject.h"
#include "Mesh.h"


class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* owner);
	MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner);
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
	std::vector<Mesh*> getMeshes() { return mMeshes; }

	//UNIFORMS
	float lightDir[3] = { 0.0f, -1.0f, 1.0f };
	float lightColor[3] = { 0.0f, 0.0f, 0.7f };
	float lightIntensity = 10.0f;

	float ambientColor[3] = {0.5f, 0.5f, 0.5f};

private:
	std::vector<Mesh*> mMeshes; //it was a mesh pointer (Mesh*) 
	OBB mOBB;
	bool* mDrawBox = new bool(false);
	bool mInsideFrustum = true;
};
