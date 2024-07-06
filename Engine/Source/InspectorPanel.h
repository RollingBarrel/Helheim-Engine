#pragma once
#include "Panel.h"
#include <vector>
#include <string>

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
class TextComponent;
class BoxColliderComponent;
class TrailComponent;
class BezierCurve;
class DecalComponent;
class RandomFloat;


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
	bool mTagsLayersPopUp = false;

	void ShowTagsLayerPopUp();

	void DrawTransform(GameObject* object);
	void AddComponentButton(GameObject* object);
	void ShowSameComponentPopup();
	void RightClickPopup(Component* component);
	void DrawComponents(GameObject* object);

	void DrawCameraComponent(CameraComponent* component);
	void DrawScriptComponent(ScriptComponent* component);
	void DrawPointLightComponent(PointLightComponent* component);
	void DrawSpotLightComponent(SpotLightComponent* component);
	void DrawMeshRendererComponent(MeshRendererComponent& component);
	void DrawAIAgentComponent(AIAgentComponent* component);
	void DrawImageComponent(ImageComponent* component);
	void DrawCanvasComponent(CanvasComponent* component);
	void DrawButtonComponent(ButtonComponent* component);
	void DrawSliderComponent(SliderComponent* component);
	void DrawAudioSourceComponent(AudioSourceComponent* component);
	void DrawListenerComponent(AudioListenerComponent* component);
	void DrawTransform2DComponent(Transform2DComponent* component);
	void DrawTextComponent(TextComponent* component);
	void DrawBoxColliderComponent(BoxColliderComponent* component);
	void DrawParticleSystemComponent(ParticleSystemComponent* component) const;
	void DrawTrailComponent(TrailComponent* component) const;
	void DrawDecalComponent(DecalComponent* component);

	//void DragAndDropSource(Component* component);
	//void DragAndDropTarget(GameObject* object, Component* target);
	void MaterialVariables(const MeshRendererComponent& renderComponent);
	void DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component);
	void DrawAnimationComponent(AnimationComponent* component);
	void GetStateMachineAssets(AnimationComponent* component, bool isSpine, std::vector<std::string>& names);
	void DrawBezierCurve(BezierCurve* curve, const char* cLabel) const;
	void DrawRandomFloat(RandomFloat& value, const char* cLabel) const;
	void DrawBlendTypeSelector(int& type, const char* cLabel) const;
};
