#pragma once
#include "Panel.h"
#define INSPECTORPANEL "Inspector##"

class GameObject;
class Component;
class TestComponent;
class MeshRendererComponent;
class CameraComponent;
class ScriptComponent;
class PointLightComponent;
class SpotLightComponent;
<<<<<<< HEAD
class AIAgentComponent;
class NavMeshObstacleComponent;
class AnimationComponent;
=======
class ImageComponent;
>>>>>>> 2cbcff0 (First modifications on ImageComponent)

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();
	void Draw(int windowFlags) override;

private:
	bool mSameComponentPopup = false;
	Component* mComponent = nullptr;

	bool mLocked = false;
	GameObject* mLockedGameObject = nullptr;

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	void ShowSameComponentPopup();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);
	void DrawTestComponent(TestComponent* component);
	void DrawCameraComponent(CameraComponent* component);
	void DrawScriptComponent(ScriptComponent* component);
	void DrawPointLightComponent(PointLightComponent* component);
	void DrawSpotLightComponent(SpotLightComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent* component);
<<<<<<< HEAD
	void DrawAIAgentComponent(AIAgentComponent* component);
=======
	void DrawImageComponent(ImageComponent* component);
>>>>>>> 2cbcff0 (First modifications on ImageComponent)
	void DragAndDropSource(Component* component);
	void DragAndDropTarget(GameObject* object, Component* target);
	void MaterialVariables(MeshRendererComponent* renderComponent);
	void DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component);
	void DrawAnimationComponent(AnimationComponent* component);
};
