#pragma once
#include "Component.h"
#include "Geometry/OBB.h"
#include "GameObject.h"

class Material;
struct Mesh;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(GameObject* ownerGameObject);
	MeshRendererComponent(const MeshRendererComponent& original);
	//~MeshRendererComponent();

	void Draw();
	void Reset() override;
	void Load();

	void Update() override;
	void DrawEditor() override;
	Component* Clone() override;

	const OBB getOBB() const { return mOBB; }

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();

	Mesh* mMesh;
	Material* material;
	OBB mOBB;
};

