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
class AIAgentComponent;
class NavMeshObstacleComponent;
class AnimationComponent;
class ImageComponent;
class CanvasComponent;
class ButtonComponent;
class AudioSourceComponent;
class AudioListenerComponent;
class Transform2DComponent;
class SliderComponent;
class ParticleSystemComponent;
class BoxColliderComponent;

class InspectorPanel : public Panel
{
public:
	InspectorPanel();
	//~InspectorPanel();
	void Draw(int windowFlags) override;

private:
	
	GameObject* mLockedGameObject = nullptr;
	Component* mComponent = nullptr;

	bool mLocked = false;
	bool mSameComponentPopup = false;

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
	void DrawAIAgentComponent(AIAgentComponent* component);
	void DrawImageComponent(ImageComponent* component);
	void DrawCanvasComponent(CanvasComponent* component);
	void DrawButtonComponent(ButtonComponent* component);
	void DrawAudioSourceComponent(AudioSourceComponent* component);
	void DrawListenerComponent(AudioListenerComponent* component);
	void DrawTransform2DComponent(Transform2DComponent* component);
	void DrawParticleSystemComponent(ParticleSystemComponent* component);
	void DrawBoxColliderComponent(BoxColliderComponent* component);

	void DragAndDropSource(Component* component);
	void DragAndDropTarget(GameObject* object, Component* target);
	void MaterialVariables(MeshRendererComponent* renderComponent);
	void DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component);
	void DrawAnimationComponent(AnimationComponent* component);
};
