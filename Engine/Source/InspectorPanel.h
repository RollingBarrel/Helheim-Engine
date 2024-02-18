#pragma once
#include "Panel.h"
#define INSPECTORPANEL "Inspector##"

class GameObject;
class Component;
class TestComponent;
class MeshRendererComponent;
class LightSourceComponent;

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();
	void Draw(int windowFlags) override;

private:
	bool mSameComponentPopup = false;
	Component* mComponent = nullptr;
	GameObject* lastFocusedObject = nullptr;

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	void ShowSameComponentPopup();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawLightSourceComponent(LightSourceComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
	void DragAndDropSource(Component* component);
	void DragAndDropTarget(GameObject* object, Component* target);
	void MaterialVariables(MeshRendererComponent* renderComponent);
};
