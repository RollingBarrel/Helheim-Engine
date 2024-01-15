#pragma once
#include "Panel.h"
#include "Component.h"
#define INSPECTORPANEL "Inspector##"

class GameObject;
class TestComponent;
class MeshRendererComponent;

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();

	void Draw(int windowFlags) override;
private:
	static bool mSame_component_window;

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	bool CheckComponent(GameObject* object, ComponentType type);
	static void ShowSameComponentWindow();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
	void DragAndDropSource(Component* component);
	void DragAndDropTarget(GameObject* object, Component* target);
};
