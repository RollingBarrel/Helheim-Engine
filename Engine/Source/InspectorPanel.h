#pragma once
#include "Panel.h"
#define INSPECTORPANEL "Inspector##"

class GameObject;
class Component;
class TestComponent;
class MeshRendererComponent;

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();

	void Draw(int windowFlags) override;
private:
	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
};
