#pragma once
#include "Component.h"
#include "Geometry/AABB.h"

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

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();
	void RightClickPopup() override;
	Mesh* mMesh;
	Material* material;
	AABB mAABB;
};

