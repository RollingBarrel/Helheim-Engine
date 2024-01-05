#pragma once
#include "Component.h"
#include "Geometry/AABB.h"

class Material;
struct Mesh;

class TestComponent : public Component
{
public:
	TestComponent(GameObject* ownerGameObject);
	TestComponent(const TestComponent& original);
	void Reset();
	//~TestComponent();

	void Draw();
	void Load();

	void Update() override;
	void DrawEditor() override;
	Component* Clone() override;

private:
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();

	Mesh* mMesh;
	Material* material;
	AABB mAABB;
};

