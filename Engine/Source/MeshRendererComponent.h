#pragma once
#include "Component.h"
#include "Geometry/AABB.h"

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
	void DrawEditor() override;
	Component* Clone(GameObject* owner) override;

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();
	void RightClickPopup() override;
	Mesh* mMesh;
	Material* material;
	AABB mAABB;
};

