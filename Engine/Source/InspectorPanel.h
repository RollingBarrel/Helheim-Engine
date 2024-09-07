#pragma once
#include "Panel.h"
#include <vector>
#include <string>

#define INSPECTORPANEL "Inspector##"

class GameObject;
class Component;
class MeshRendererComponent;
class CameraComponent;
class ScriptComponent;
class PointLightComponent;
class SpotLightComponent;
class AIAgentComponent;
class NavMeshObstacleComponent;
class AnimationComponent;
class ImageComponent;
class MaskComponent;
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
class LineComponent;
class DecalComponent;
class VideoComponent;
class BezierCurve;
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

	void DrawCameraComponent(CameraComponent* component) const;
	void DrawScriptComponent(ScriptComponent* component) const;
	void DrawPointLightComponent(PointLightComponent* component) const;
	void DrawSpotLightComponent(SpotLightComponent* component) const;
	void DrawMeshRendererComponent(MeshRendererComponent& component) const;
	void DrawAIAgentComponent(AIAgentComponent* component) const;
	void DrawImageComponent(ImageComponent* component) const;
	void DrawMaskComponent(MaskComponent* component) const;
	void DrawCanvasComponent(CanvasComponent* component) const;
	void DrawButtonComponent(ButtonComponent* component) const {}
	void DrawSliderComponent(SliderComponent* component) const;
	void DrawAudioSourceComponent(AudioSourceComponent* component) const;
	void DrawListenerComponent(AudioListenerComponent* component) const {}
	void DrawTransform2DComponent(Transform2DComponent* component) const;
	void DrawTextComponent(TextComponent* component) const;
	void DrawBoxColliderComponent(BoxColliderComponent* component) const;
	void DrawParticleSystemComponent(ParticleSystemComponent* component) const;
	void DrawTrailComponent(TrailComponent* component) const;
	void DrawLineComponent(LineComponent* component) const;
	void DrawDecalComponent(DecalComponent* component) const;
	void DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component) const;
	void DrawAnimationComponent(AnimationComponent* component) const;
	void DrawVideoComponent(VideoComponent* component) const;

	//void DragAndDropSource(Component* component);
	//void DragAndDropTarget(GameObject* object, Component* target);
	void MaterialVariables(const MeshRendererComponent& renderComponent) const;
	void GetStateMachineAssets(AnimationComponent* component, bool isSpine, std::vector<std::string>& names) const;
	void DrawBezierCurve(BezierCurve* curve, const char* cLabel) const;
	void DrawRandomFloat(RandomFloat& value, const char* cLabel) const;
	void DrawBlendTypeSelector(int& type, const char* cLabel) const;
};
