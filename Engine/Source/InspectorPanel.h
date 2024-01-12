#pragma once
#include "Panel.h"
#include "Component.h"
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
	static bool mSame_component_window;

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	static void ShowSameComponentPopup();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
};
