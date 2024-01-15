#pragma once
#include "Panel.h"
#define INSPECTORPANEL "Inspector##"

class GameObject;
class Component;
class TestComponent;
class MeshRendererComponent;
enum class ComponentType : unsigned int;

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();
	void Draw(int windowFlags) override;

private:
	static bool mSame_component_popup;
	Component* mComponent = nullptr;

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	void ShowSameComponentPopup();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
	void DragAndDropSource(Component* component);
	void DragAndDropTarget(GameObject* object, Component* target);
};
