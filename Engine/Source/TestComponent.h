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

	void Update() override;
	void DrawEditor() override;
	Component* Clone() override;

private:
	void RightClickPopup() override;
};

