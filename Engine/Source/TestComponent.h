#pragma once
#include "Component.h"
#include "Geometry/AABB.h"
#include "GameObject.h"

class Material;
struct Mesh;

class TestComponent : public Component
{
public:
	TestComponent(GameObject* ownerGameObject);
	TestComponent(const TestComponent& original, GameObject* owner);
	void Reset();
	//~TestComponent();

	void Draw();
	void Load();

	void Update() override;
	Component* Clone(GameObject* owner) override;

	void Save(Archive& archive) const override;
	void Load(const rapidjson::Value& data) override;

private:
	int number;
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();

	Mesh* mMesh;
	Material* material;
	AABB mAABB;
};

