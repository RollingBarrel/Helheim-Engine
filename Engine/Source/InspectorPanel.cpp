#include "InspectorPanel.h"

#include "ImBezier.h"
#include "imgui.h"
#include "imgui_color_gradient.h"

#include "EngineApp.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "HierarchyPanel.h"
#include "TagsManagerPanel.h"
#include "ProjectPanel.h"
#include "ModuleCamera.h"
#include "ModuleScriptManager.h"
#include "ModuleAudio.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"
#include "ModuleUI.h"
#include "GameObject.h"

#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "CameraComponent.h"
#include "AIAGentComponent.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "ButtonComponent.h"
#include "AudioSourceComponent.h"
#include "Transform2DComponent.h"
#include "ParticleSystemComponent.h"
#include "TextComponent.h"
#include "TrailComponent.h"
#include "EmitterShape.h"
#include "BoxColliderComponent.h"
#include "NavMeshObstacleComponent.h"
#include "AnimationComponent.h"
#include "SliderComponent.h"

#include "ImporterMaterial.h"
#include "Tag.h"
#include "MathFunc.h"
#include "Script.h"
#include "AnimationController.h"
#include "BezierCurve.h"
#include "Trail.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceAnimation.h"
#include "ResourceModel.h"

#include "IconsFontAwesome6.h"


#include "AnimationStateMachine.h"

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	HierarchyPanel* hierarchyPanel = (HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL);
	GameObject* focusedObject = hierarchyPanel->GetFocusedObject();

	if (mLockedGameObject != nullptr) 
	{
		focusedObject = mLockedGameObject;
	}

	if (focusedObject == nullptr) return;

	char nameArray[100];
	strcpy_s(nameArray, focusedObject->mName.c_str());
	bool enabled = focusedObject->IsEnabled();
	ImGui::PushID(focusedObject->mID);
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (!focusedObject->IsRoot())
	{
		if (ImGui::Checkbox("##enabled", &enabled))
		{
			focusedObject->SetEnabled(enabled);
		}
		ImGui::SameLine();


		// Rename
		ImGui::PushID(focusedObject->mID);
		ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
		focusedObject->mName = nameArray;
		ImGui::PopID();

		// Lock
		ImVec2 buttonSize = ImVec2(30, 20);
		//float inspectorWidth = ImGui::GetWindowWidth();
		//float spacing = (inspectorWidth / 2) - buttonSize.x;
		//ImGui::SameLine();
		//ImGui::Dummy(ImVec2(spacing, 0.0f));
		ImGui::SameLine();
		//ImGui::PushItemWidth(inspectorWidth - buttonSize.x - spacing);
		const char* buttonLabel = (mLocked) ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN;
		ImGui::SameLine();
		if (ImGui::Button(buttonLabel, buttonSize))
		{
			mLocked = !mLocked;
			if (mLocked) {
				mLockedGameObject = focusedObject;
			}
			else
			{
				mLockedGameObject = nullptr;
			}
		}
		//ImGui::PopItemWidth();
		bool dynamic = focusedObject->IsDynamic();
		if (ImGui::Checkbox("Dynamic",&dynamic))
		{
			focusedObject->SetDynamic(dynamic);
		}
		// Tag
		ImGui::Text("Tag");
		ImGui::SameLine();
		std::vector<Tag*> tags = EngineApp->GetScene()->GetAllTags();

		if (ImGui::BeginCombo("##tags", focusedObject->GetTag()->GetName().c_str()))
		{
			for (auto i = 0; i < tags.size(); i++) 
			{
				if (ImGui::Selectable(tags[i]->GetName().c_str()))
				{
					focusedObject->SetTag(tags[i]);
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (ImGui::Button("Add Tag")) 
		{
			EngineApp->GetEditor()->OpenPanel(TAGSMANAGERPANEL, true);
		}

		if (focusedObject->mPrefabResourceId != 0) {
			ImGui::Text("From Prefab");
			ImGui::Checkbox("Override Prefab", &focusedObject->mPrefabOverride);
		}

		DrawTransform(focusedObject);
		DrawComponents(focusedObject);
		ImGui::Separator();
		AddComponentButton(focusedObject);
	}

	if (mSameComponentPopup)
	{
		ShowSameComponentPopup();
	}

	ImGui::End();
	ImGui::PopID();
}

void InspectorPanel::DrawTransform(GameObject* object) {
	ImGui::PushID(object->mID);
	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) 
	{

		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) 
	{
		if (ImGui::Selectable("Reset")) 
		{
			object->ResetTransform();
		}
		ImGui::EndPopup();
	}

	if (headerOpen) 
	{
		if (ImGui::BeginTable("transformTable", 4)) 
		{
			//ImGui::TableSetupColumn("columns", 0 , -FLT_MIN);

			bool modifiedTransform = false;
			float3 newPosition = object->mPosition;
			float3 newRotation = RadToDeg(object->mEulerRotation);
			float3 newScale = object->mScale;

			const char* labels[3] = { "Position", "Rotation", "Scale" };
			const char* axisLabels[3] = { "X", "Y", "Z" };
			float3* vectors[3] = { &newPosition, &newRotation, &newScale };

			for (int i = 0; i < 3; ++i) 
			{
				ImGui::PushID(i);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Text(labels[i]);
				ImGui::PopItemWidth();

				for (int j = 0; j < 3; ++j) 
				{
					ImGui::TableNextColumn();
					ImGui::PushItemWidth(-FLT_MIN);
					ImGui::AlignTextToFramePadding();
					ImGui::Text(axisLabels[j]);
					ImGui::SameLine();
					modifiedTransform = ImGui::DragFloat(axisLabels[j], &(*vectors[i])[j], 0.05f, 0.0f, 0.0f, "%.2f") || modifiedTransform;
					ImGui::PopItemWidth();
				}
				ImGui::PopID();
			}

			if (modifiedTransform) 
			{

				object->SetPosition(newPosition);
				object->SetRotation(DegToRad(newRotation));
				object->SetScale(newScale);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopID();
}

void InspectorPanel::AddComponentButton(GameObject* object) {
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::SetCursorPosX(posX);
	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) 
	{
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) 
	{
		for (unsigned int i = 0; i <= static_cast<unsigned int>(ComponentType::NONE) - 1; ++i) 
		{
			ComponentType currentComponent = static_cast<ComponentType>(i);
			if (ImGui::MenuItem(Component::GetNameFromType(currentComponent))) 
			{
				mComponent = object->GetComponent(currentComponent);
				if (!mComponent)
				{
					object->CreateComponent(currentComponent);
				}
				else 
				{
					mSameComponentPopup = true;
				}
			}
		}
		ImGui::EndPopup();
	}
}

void InspectorPanel::ShowSameComponentPopup()
{
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImVec2 centerPos = ImVec2(mainViewport->Pos.x + mainViewport->Size.x * 0.5f,
		mainViewport->Pos.y + mainViewport->Size.y * 0.5f);

	ImGui::SetNextWindowPos(centerPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(380, 107), ImGuiCond_Appearing);

	ImGui::Begin("Can't add the same component multiple times!", &mSameComponentPopup,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("The component %s can't be added because", Component::GetNameFromType(mComponent->GetType()));
	ImGui::Text("GameObject already contains the same component.");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	float buttonPosX = ImGui::GetWindowContentRegionMax().x - 120;

	ImGui::SetCursorPosX(buttonPosX);

	if (ImGui::Button("Cancel", ImVec2(120, 25))) 
	{
		mSameComponentPopup = false;
	}

	ImGui::End();
}

/****************** COMPONENTS ********************/

void InspectorPanel::RightClickPopup(Component* component) {

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) 
	{
		ImGui::OpenPopup(std::to_string(component->GetID()).c_str());
	}

	if (ImGui::BeginPopup(std::to_string(component->GetID()).c_str())) 
	{
		if (ImGui::MenuItem("Delete Component")) 
		{
			component->mOwner->AddComponentToDelete(component);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Reset Component")) 
		{
			component->Reset();
			ImGui::CloseCurrentPopup();
		}

		switch (component->GetType()) 
		{
		case ComponentType::TEST: 
		{
			if (ImGui::MenuItem("Custom Test Component Option")) 
			{
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Custom Test Component Option 2")) 
			{
				ImGui::CloseCurrentPopup();
			}
			break;
		}
		case ComponentType::MESHRENDERER: 
		{
			if (ImGui::MenuItem("Custom MeshRendererComponent Option")) 
			{
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Custom MeshRendererComponent Option")) 
			{
				ImGui::CloseCurrentPopup();
			}
			break;
		}
		}

		ImGui::EndPopup();
	}

}

void InspectorPanel::DragAndDropSource(Component* component) {
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("_COMPONENT", component, sizeof(*component));

		ImGui::Text(Component::GetNameFromType(component->GetType()));
		ImGui::EndDragDropSource();
	}
}

void InspectorPanel::DragAndDropTarget(GameObject* object, Component* target) {
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::InvisibleButton("##", ImVec2(-1, 5));
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COMPONENT"))
		{
			Component* movedComponent = (Component*)payload->Data;
			if (target != nullptr ? movedComponent->GetID() != target->GetID() : true) 
			{
				Component* pMovedComponent = object->RemoveComponent(movedComponent);
				object->AddComponent(pMovedComponent, target);
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopStyleVar();
}

void InspectorPanel::DrawComponents(GameObject* object) 
{
	for (auto component : object->mComponents) 
	{
		ImGui::PushID(component->mID);
		DragAndDropTarget(object, component);

		bool isOpen = ImGui::CollapsingHeader(Component::GetNameFromType(component->GetType()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		RightClickPopup(component);

		bool isEnabled = component->IsEnabled();
		if (ImGui::Checkbox("Enable", &isEnabled)) 
		{
			component->SetEnable(isEnabled);
		}

		DragAndDropSource(component);
		
		if (isOpen) 
		{
			switch (component->GetType()) 
			{
				case ComponentType::MESHRENDERER:
					DrawMeshRendererComponent(reinterpret_cast<MeshRendererComponent*>(component));
					break;
				case ComponentType::AIAGENT:
					DrawAIAgentComponent(reinterpret_cast<AIAgentComponent*>(component));
					break;
				case ComponentType::POINTLIGHT:
					DrawPointLightComponent(reinterpret_cast<PointLightComponent*>(component));
					break;
				case ComponentType::SPOTLIGHT:
					DrawSpotLightComponent(reinterpret_cast<SpotLightComponent*>(component));
					break;
				case ComponentType::CAMERA:
					DrawCameraComponent(reinterpret_cast<CameraComponent*>(component));
					break;
				case ComponentType::SCRIPT:
					DrawScriptComponent(reinterpret_cast<ScriptComponent*>(component));
					break;
				case ComponentType::NAVMESHOBSTACLE: 
					DrawNavMeshObstacleComponent(reinterpret_cast<NavMeshObstacleComponent*>(component));
					break;
				case ComponentType::ANIMATION: 
					DrawAnimationComponent(reinterpret_cast<AnimationComponent*>(component));
					break;
				case ComponentType::TEST: 
					DrawTestComponent(reinterpret_cast<TestComponent*>(component));
					break;
				case ComponentType::IMAGE:
					DrawImageComponent(reinterpret_cast<ImageComponent*>(component));
					break;
				case ComponentType::CANVAS:
					DrawCanvasComponent(reinterpret_cast<CanvasComponent*>(component));
					break;
				case ComponentType::SLIDER:
					DrawSliderComponent(reinterpret_cast<SliderComponent*>(component));
					break;
				case ComponentType::BUTTON:
					DrawButtonComponent(reinterpret_cast<ButtonComponent*>(component));
					break;
				case ComponentType::AUDIOSOURCE: 
					DrawAudioSourceComponent(reinterpret_cast<AudioSourceComponent*>(component));
					break;
				case ComponentType::TRANSFORM2D:
					DrawTransform2DComponent(reinterpret_cast<Transform2DComponent*>(component));
					break;
				case ComponentType::PARTICLESYSTEM:
					DrawParticleSystemComponent(reinterpret_cast<ParticleSystemComponent*>(component));
					break;
				case ComponentType::TEXT:
					DrawTextComponent(reinterpret_cast<TextComponent*>(component));
					break;
				case ComponentType::BOXCOLLIDER:
					DrawBoxColliderComponent(reinterpret_cast<BoxColliderComponent*>(component));
					break;
				case ComponentType::TRAIL:
					DrawTrailComponent(reinterpret_cast<TrailComponent*>(component));
					break;
			}
		}
		ImGui::PopID();
	}
	DragAndDropTarget(object, nullptr);
}

void InspectorPanel::DrawTestComponent(TestComponent* component) 
{
	ImGui::Text("Demo Text");
	ImGui::Text("Demo Text 2 ");

	ImGui::SeparatorText("TAGS SYSYEM TEST");
	ImGui::Text("The first name of game object found with");
	ImGui::SameLine();
	std::vector<Tag*> tags = EngineApp->GetScene()->GetAllTags();

	if (ImGui::BeginCombo("##tags", tags[component->mTestSavedTag1]->GetName().c_str()))
	{
		for (auto i = 0; i < tags.size(); i++) 
		{
			if (ImGui::Selectable(tags[i]->GetName().c_str()))
			{
				component->mTestSavedTag1 = i;
			}
		}

		ImGui::EndCombo();
	}
	ImGui::Text("tag is ");
	ImGui::SameLine();
	GameObject* found = GameObject::FindGameObjectWithTag(tags[component->mTestSavedTag1]->GetName());
	if (found != nullptr)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), found->GetName().c_str());
	}
	else 
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not found");
	}


	ImGui::Separator();
	ImGui::Text("There is ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), std::to_string(GameObject::FindGameObjectsWithTag(tags[component->mTestSavedTag1]->GetName()).capacity()).c_str());
	ImGui::SameLine();
	ImGui::Text(" gameobjects with the same tag.");
}

void InspectorPanel::DrawPointLightComponent(PointLightComponent* component) 
{
	const float* pCol = component->GetColor();
	float col[3] = { pCol[0], pCol[1] , pCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 0.5f, 0.0f, 300.f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 0.5f, 0.0f, 1000.f))
	{
		component->SetRadius(radius);
	}
	//ImGui::Checkbox("Debug draw", &component->debugDraw);
}

void InspectorPanel::DrawSpotLightComponent(SpotLightComponent* component) 
{
	const float* sCol = component->GetColor();
	float col[3] = { sCol[0], sCol[1] , sCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 0.5f, 0.0f, 300.f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRange();
	if (ImGui::DragFloat("Range", &radius, 0.5f, 0.0f, 1000.f))
	{
		component->SetRange(radius);
	}
	float innerAngle = RadToDeg(component->GetInnerAngle());
	float outerAngle = RadToDeg(component->GetOuterAngle());
	if (ImGui::DragFloat("Inner angle", &innerAngle, 1.0f, 0.0f, outerAngle))
	{
		component->SetInnerAngle(DegToRad(innerAngle));
	}
	if (ImGui::DragFloat("Outer angle", &outerAngle, 1.0f, innerAngle, 75.0f))
	{
		component->SetOuterAngle(DegToRad(outerAngle));
	}

	ImGui::SeparatorText("Shadows");

	bool castShadow = component->CanCastShadow();
	if (ImGui::Checkbox("Cast Shadow", &castShadow))
	{
		component->SetCastShadow(castShadow);
	}

	if (!castShadow)
	{
		ImGui::BeginDisabled();
	}

	float bias = component->GetBias();
	if (ImGui::DragFloat("Bias", &bias, 0.00001f, 0.0f, 1.0, "%.5f"))
	{
		component->SetBias(bias);
	}

	if (!castShadow)
	{
		ImGui::EndDisabled();
	}

}

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent* component) 
{
	ImGui::SeparatorText("Material");

	MaterialVariables(component);

	ImGui::Text(" ");
	//TODO: SEPARATE GAME ENGINE
	//bool shouldDraw = component->ShouldDraw();
	//if (ImGui::Checkbox("Draw bounding box:", &shouldDraw)) {
	//	component->SetShouldDraw(shouldDraw);
	//}
}

void InspectorPanel::DrawAIAgentComponent(AIAgentComponent* component)
{
	//ImGui::SeparatorText("Agent Parameters");

	//float radius = component->GetRadius();
	//if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	//{
	//	component->SetRadius(radius);
	//}
	//float height = component->GetHeight();
	//if (ImGui::DragFloat("Height", &height, 1.0f, 0.0f))
	//{
	//	component->SetHeight(height);
	//}
	//float stepHeight = component->GetStepHeight();
	//if (ImGui::DragFloat("StepHeight", &stepHeight, 1.0f, 0.0f))
	//{
	//	component->SetStepHeight(stepHeight);
	//}

	//int maxSlope = component->GetMaxSlope();
	//if (ImGui::SliderInt("Max Slope", &maxSlope, 0, 60)) {
	//	component->SetMaxSlope(maxSlope);
	//}

	//ImGui::SeparatorText("Steering Parameters");

	//float speed = component->GetSpeed();
	//if (ImGui::DragFloat("Speed", &speed, 1.0f, 0.0f,0.0f))
	//{
	//	component->SetSpeed(speed);
	//}

	/*float angularSpeed = component->GetAngularSpeed();
	if (ImGui::DragFloat("Angular Speed", &angularSpeed, 1.0f, 0.0f))
	{
		component->SetAngularSpeed(angularSpeed);
	}

	float acceleration = component->GetAcceleration();
	if (ImGui::DragFloat("acceleration", &acceleration, 1.0f, 0.0f))
	{
		component->SetAcceleration(acceleration);
	}

	float stoppingDistance = component->GetStoppingDistance();
	if (ImGui::DragFloat("Stopping Distance", &stoppingDistance, 1.0f, 0.0f))
	{
		component->SetStoppingDistance(stoppingDistance);
	}*/



}

void InspectorPanel::MaterialVariables(MeshRendererComponent* renderComponent)
{
	ResourceMaterial* material = const_cast<ResourceMaterial*>(renderComponent->GetResourceMaterial());

	if (material)
	{
		if (ImGui::Checkbox("Enable BaseColor map", &material->mEnableBaseColorTexture))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (ImGui::Checkbox("Enable MetallicRoughness map", &material->mEnableMetallicRoughnessTexture))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (ImGui::Checkbox("Enable Normal map", &material->mEnableNormalMap))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (ImGui::Checkbox("Enable Emissive map", &material->mEnableEmissiveTexture))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}

		if (ImGui::ColorPicker3("BaseColor", material->mBaseColorFactor.ptr()))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (ImGui::DragFloat("Metalnes", &material->mMetallicFactor, 0.01f, 0.0f, 1.0f, "%.2f"))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (ImGui::DragFloat("Roughness", &material->mRoughnessFactor, 0.01f, 0.0f, 1.0f, "%.2f"))
		{
			EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
		if (material->IsEmissiveEnabled())
		{
			if (ImGui::ColorPicker3("Emissive", material->mEmissiveFactor.ptr()))
			{
				EngineApp->GetOpenGL()->BatchEditMaterial(renderComponent);
			}
		}
	}
}

void InspectorPanel::DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component)
{
	ImGui::SeparatorText("Navigation Mesh Obstacle");

	float Radius = component->GetRadius();
	ImGui::InputFloat("Radius", &Radius);
	component->SetRadius(Radius);

	float Height = component->GetHeight();
	ImGui::InputFloat("Height", &Height);
	component->SetHeight(Height);

}


void InspectorPanel::DrawCameraComponent(CameraComponent* component)
{
	ImGui::SeparatorText("Camera");

	static float nearPlane = component->GetNearPlane();
	const char* nearLabel = "##NearPlane";

	static float farPlane = component->GetFarPlane();
	const char* farLabel = "##FarPlane";

	static float FOV = RadToDeg(component->GetVerticicalFOV());
	const char* FOVLabel = "##FOV";

	ImGui::PushID(nearLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Near Plane");
	ImGui::SameLine();
	if (ImGui::DragFloat(nearLabel, &nearPlane, 0.05f, 0.0f, 2000, "%.2f"))
	{
		component->SetNearPlane(nearPlane);
	}
	ImGui::PopID();

	ImGui::PushID(farLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Far Plane");
	ImGui::SameLine();
	if (ImGui::DragFloat(farLabel, &farPlane, 0.05f, 0.0f, 2000, "%.2f"))
	{
		component->SetFarPlane(farPlane);
	}
	ImGui::PopID();

	ImGui::PushID(FOVLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("FOV");
	ImGui::SameLine();
	if (ImGui::DragFloat(FOVLabel, &FOV, 0.05f, 0.0f, 2000, "%.2f"))
	{
		component->SetFOV(FOV);
	}
	ImGui::PopID();
}

void InspectorPanel::DrawScriptComponent(ScriptComponent* component)
{

	const char* currentItem = component->GetScriptName();
	if (strcmp(currentItem, "") == 0)
	{
		currentItem = "None (Script)";
	}
	
	ImVec2 scriptTextSize = ImGui::CalcTextSize("Script");
	float inspectorWidth = ImGui::GetWindowWidth();
	float scriptSpacing = (inspectorWidth / 2) - scriptTextSize.x;


	ImGui::Text("Script");
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(scriptSpacing, 0.0f));
	ImGui::SameLine();
	ImGui::PushItemWidth(inspectorWidth - scriptTextSize.x - scriptSpacing);
	if (ImGui::BeginCombo("##combo", currentItem)) 
	{
		std::vector<std::string> scriptNames;
		EngineApp->GetFileSystem()->DiscoverFiles(ASSETS_SCRIPT_PATH, ".emeta", scriptNames);
		for (int i = 0; i < scriptNames.size(); ++i)
		{
			
			size_t slashPos = scriptNames[i].find_last_of('/');
			if (slashPos != std::string::npos)
			{
				
				scriptNames[i].erase(0, slashPos + 1);
			}
			
			size_t dotPos = scriptNames[i].find_first_of('.');
			if (dotPos != std::string::npos)
			{
				
				scriptNames[i].erase(dotPos);
			}
		}

		for (int n = 0; n < scriptNames.size(); n++)
		{
			bool is_selected = (currentItem == scriptNames[n]); 
			if (ImGui::Selectable(scriptNames[n].c_str(), is_selected)) 
			{
				currentItem = scriptNames[n].c_str();
				component->LoadScript(currentItem);
				currentItem = component->GetScriptName();

				if (component->mScript)
				{
					component->mScript->Awake();
				}
			}

			if (is_selected) 
			{
				ImGui::SetItemDefaultFocus();
			}

		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
	ImGui::Dummy(ImVec2(0,5.0f));

	component->mScript;
	std::vector<std::pair<std::string, std::pair<MemberType, void*>>> variables;


	std::vector<Member*> members;

	if (component->mScript) 
	{
		members = component->mScript->mMembers;
	}

	for (Member* member : members) 
	{
		std::string label = member->mName;
		label = "##" + label;
		ImVec2 textSize = ImGui::CalcTextSize(member->mName);
		float inspectorWidth = ImGui::GetWindowWidth();
		float spacing = (inspectorWidth / 2) - textSize.x;

		switch (member->mType)
		{
		case MemberType::SEPARATOR:
			ImGui::SeparatorText(member->mName);
			break;
		case MemberType::INT:
			ImGui::Text(member->mName);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f));
			ImGui::SameLine();
			ImGui::PushItemWidth(inspectorWidth - textSize.x - spacing);
			ImGui::DragInt(label.c_str(), reinterpret_cast<int*>((((char*)component->mScript) + member->mOffset)));
			ImGui::PopItemWidth();
			break;
		case MemberType::FLOAT:
			ImGui::Text(member->mName);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f));
			ImGui::SameLine();
			ImGui::PushItemWidth(inspectorWidth - textSize.x - spacing);
			ImGui::DragFloat(label.c_str(), reinterpret_cast<float*>((((char*)component->mScript) + member->mOffset)));
			ImGui::PopItemWidth();
			break;
		case MemberType::BOOL:
			ImGui::Text(member->mName);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f));
			ImGui::SameLine();
			ImGui::PushItemWidth(inspectorWidth - textSize.x - spacing);
			ImGui::Checkbox(label.c_str(), reinterpret_cast<bool*>((((char*)component->mScript) + member->mOffset)));
			ImGui::PopItemWidth();
			break;
		case MemberType::FLOAT3:
			ImGui::Text(member->mName);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f));
			ImGui::SameLine();
			ImGui::PushItemWidth(inspectorWidth - textSize.x - spacing);
			ImGui::DragFloat3(label.c_str(), reinterpret_cast<float*>((((char*)component->mScript) + member->mOffset)), 0.05f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
			break;
		case MemberType::GAMEOBJECT:
		{
			
			GameObject** gameObject = reinterpret_cast<GameObject**>((((char*)component->mScript) + member->mOffset));
			const char* str = "";
			if (!gameObject || !*gameObject)
			{
				str = "None (Game Object)";
			}
			else 
			{
				str = (*gameObject)->GetName().c_str();
			}

			ImGui::Text(member->mName);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(spacing, 0.0f));
			ImGui::SameLine();
			ImGui::PushItemWidth(inspectorWidth - textSize.x - spacing);
			ImGui::BulletText(str);
			if (ImGui::BeginDragDropTarget()) 
			{

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) 
				{
					if (gameObject) 
					{
						*gameObject = *(GameObject**)payload->Data;
					}
					
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopItemWidth();
			break;
		}
		default:
			break;
		}
	}
}


void InspectorPanel::DrawAnimationComponent(AnimationComponent* component) 
{

	ImGui::SeparatorText("Animation");

	GameObject* owner = const_cast<GameObject*>(component->GetOwner());
	std::vector<Component*> components = owner->FindComponentsInChildren(owner, ComponentType::MESHRENDERER);

	bool loop = component->GetLoop();
	//bool play = false;


	if (component->GetModelUUID() != 0)
	{
		ImGui::Text("Current state: ");
		ImGui::SameLine();
		ImGui::Text(component->GetCurrentStateName().c_str());
	
		if (ImGui::Button("Play/Pause"))
		{
			//component->OnStart();
			bool play = component->GetIsPlaying();
			component->SetIsPlaying(!play);
		}

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(40.0f, 0.0f));
		ImGui::SameLine();



		if (component->GetIsPlaying())
		{
			ImGui::Text("PLAYING");
		}
		else
		{
			ImGui::Text("PAUSED");
		}

		if (ImGui::Button("Restart"))
		{
			component->OnRestart();
		}

		if (ImGui::Checkbox("Loop", &loop))
		{
			component->SetLoop(loop);
		}

		float animSpeed = component->GetAnimSpeed();

		if (ImGui::DragFloat("Animation Speed", &animSpeed, 0.02f, 0.0f, 2.0f))
		{
			component->SetAnimSpeed(animSpeed);
		}
	}
	else
	{
		const char* currentItem = "None";
		if (ImGui::BeginCombo("##combo", currentItem))
		{
			std::vector<std::string> modelNames;
			EngineApp->GetFileSystem()->DiscoverFiles("Assets/Models", ".emeta", modelNames);
			for (int i = 0; i < modelNames.size(); ++i)
			{

				size_t slashPos = modelNames[i].find_last_of('/');
				if (slashPos != std::string::npos)
				{

					modelNames[i].erase(0, slashPos + 1);
				}

				size_t dotPos = modelNames[i].find_first_of('.');
				if (dotPos != std::string::npos)
				{

					modelNames[i].erase(dotPos);
				}
			}

			for (int n = 0; n < modelNames.size(); n++)
			{
				bool is_selected = (currentItem == modelNames[n]);
				if (ImGui::Selectable(modelNames[n].c_str(), is_selected))
				{
					currentItem = modelNames[n].c_str();
					ResourceModel* model = reinterpret_cast<ResourceModel*>(App->GetResource()->RequestResource(std::string("Assets/Models/" + std::string(currentItem) + ".gltf").c_str()));
					if (model)
					{
						if (model->mAnimationUids.size() > 0)
						{
							component->SetModel(model);
						}
						else
						{
							currentItem = "Error: Not animated";
							is_selected = false;
						}
						App->GetResource()->ReleaseResource(model->GetUID());
					}
					
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

			}
			ImGui::EndCombo();
		}
	}



}

void InspectorPanel::DrawImageComponent(ImageComponent* imageComponent)
{
	// Drag and drop	
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, 70.0);

	ResourceTexture* image = imageComponent->GetImage();

	if (image)
	{
		ImTextureID imageID = (void*)(intptr_t)image->GetOpenGLId();
		ImGui::Image(imageID, ImVec2(50, 50));
	}
	else
	{
		ImGui::Text("Drop Image");
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
		{
			AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
			Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
			if (resource && (resource->GetType() == Resource::Type::Texture))
			{
				imageComponent->SetImage(resource->GetUID());
				imageComponent->SetFileName(asset->mName);
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::NextColumn();
	if (imageComponent->GetFileName() != nullptr)
	{
		ImGui::Text(imageComponent->GetFileName());
	}

	if (image)
	{
		ImGui::Text("Width:%dpx", image->GetWidth());
		ImGui::Text("Height:%dpx", image->GetHeight());
	}
	ImGui::Columns(1);

	// Color and alpha
	float3* color = imageComponent->GetColor();
	float* alpha = imageComponent->GetAlpha();
	ImGui::Text("Color:"); ImGui::SameLine(); ImGui::ColorEdit3("", (float*)color);
	ImGui::Text("Alpha:"); ImGui::SameLine(); ImGui::SliderFloat("Alpha", alpha, 0.0f, 1.0f);

	// Image Info.
	//ImGui::Text("Width:%dpx", imageComponent->GImetImage()->GetWidth()); ImGui::SameLine(); ImGui::Text("Height:%dpx", imageComponent->GetImage()->GetHeight());

	if (ImGui::Checkbox("Mantain Ratio", imageComponent->GetMantainRatio()))
	{
		if (imageComponent->GetMantainRatio())
		{
			imageComponent->ResizeByRatio();
		}
	}

	// Display options based on the type of the image
	const char* items[] = { "Image", "Spritesheet" };
	int currentItem = imageComponent->IsSpritesheet() ? 1 : 0;
	if (ImGui::Combo("Image Type", &currentItem, items, IM_ARRAYSIZE(items)))
	{
		imageComponent->SetIsSpritesheet(currentItem == 1);
	}
	if (imageComponent->IsSpritesheet())
	{
		// Play/Pause/Stop buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Pale green
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text
		if (ImGui::Button("Play"))
		{
			imageComponent->PlayAnimation();
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.6f, 1.0f)); // Pale yellow
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Pause"))
		{
			imageComponent->PauseAnimation();
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.6f, 1.0f)); // Pale red
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Stop"))
		{
			imageComponent->StopAnimation();
		}
		ImGui::PopStyleColor(2);

		// Columns and rows selector
		int columns = imageComponent->GetColumns();
		int rows = imageComponent->GetRows();
		ImGui::InputInt("Columns", &columns);
		ImGui::InputInt("Rows", &rows);
		if (columns <= 0) 
		{
			columns = 1;
		}
		if (rows <= 0) 
		{
			rows = 1;
		}
		imageComponent->SetSpritesheetLayout(columns, rows);

		// Reproduction speed slider
		int reproductionSpeed = imageComponent->GetFrameDuration();
		ImGui::Text("Speed (FPS):"); ImGui::SameLine();
		ImGui::PushItemWidth(100);
		ImGui::SliderInt("Reproduction Speed", &reproductionSpeed, 1, 100);
		ImGui::PopItemWidth();
		imageComponent->SetFrameDuration(reproductionSpeed);

		// Display the spritesheet image
		if (image != nullptr)
		{
			ImVec2 imageSize(175, 175);
			ImVec4 tint(color->x, color->y, color->z, *alpha);
			ImGui::Image((void*)(intptr_t)image->GetOpenGLId(), imageSize, ImVec2(0, 0), ImVec2(1, 1), tint);

			// Draw lines to divide the image into columns and rows
			ImVec2 imagePos = ImGui::GetItemRectMin();
			float columnWidth = imageSize.x / columns;
			float rowHeight = imageSize.y / rows;
			for (int i = 1; i < columns; ++i)
			{
				ImVec2 start(imagePos.x + i * columnWidth, imagePos.y);
				ImVec2 end(imagePos.x + i * columnWidth, imagePos.y + imageSize.y);
				ImGui::GetWindowDrawList()->AddLine(start, end, IM_COL32(255, 255, 255, 255));
			}
			for (int i = 1; i < rows; ++i)
			{
				ImVec2 start(imagePos.x, imagePos.y + i * rowHeight);
				ImVec2 end(imagePos.x + imageSize.x, imagePos.y + i * rowHeight);
				ImGui::GetWindowDrawList()->AddLine(start, end, IM_COL32(255, 255, 255, 255));
			}

			// Display a list of the slices
			if (ImGui::CollapsingHeader("Slices", ImGuiTreeNodeFlags_None))
			{
				float sliceWidth = 1.0f / columns;
				float sliceHeight = 1.0f / rows;
				ImVec2 sliceSize(50, 50);
				
				for (int row = 0; row < rows; ++row)
				{
					for (int col = 0; col < columns; ++col)
					{
						// Calculate the texture coordinates for the slice
						ImVec2 uv0(col * sliceWidth, row * sliceHeight);
						ImVec2 uv1((col + 1) * sliceWidth, (row + 1) * sliceHeight);

						ImGui::Image((void*)(intptr_t)image->GetOpenGLId(), sliceSize, uv0, uv1, tint);

						if (col < columns - 1)
						{
							ImGui::SameLine();
							ImGui::Text(" ");
							ImGui::SameLine();
						}
					}
					if (row < rows - 1)
					{
						ImGui::NewLine();
					}
				}
			}
		}
	}
}


void InspectorPanel::DrawCanvasComponent(CanvasComponent* canvasComponent) 
{
	const char* renderModes[] = { "World Space", "Screen Space" };
	static int selectedRenderMode = 1;

	ImGui::Text("Render Mode");
	ImGui::SameLine();
	ImGui::Combo("##RenderModeCombo", &selectedRenderMode, renderModes, IM_ARRAYSIZE(renderModes));

	if (selectedRenderMode == 0) 
	{
		canvasComponent->SetScreenSpace(false);
	}
	else 
	{
		canvasComponent->SetScreenSpace(true);
	}

	if (ImGui::BeginTable("transformTable", 4)) 
	{
		ImGui::PushID(0);
		ImGui::TableNextRow();

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Size");
		ImGui::PopItemWidth(); 
		
		bool modifiedTransform = false;
		float2 newSize = canvasComponent->GetSize();
		const char* axisLabels2d[2] = { "Width", "Height" };

		for (int j = 0; j < 2; ++j) 
		{
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text(axisLabels2d[j]);
			modifiedTransform = ImGui::DragFloat(axisLabels2d[j], &newSize[j], 0.05f, 0.0f, 0.0f, "%.2f") || modifiedTransform;
			ImGui::PopItemWidth();
		}
		ImGui::PopID();


		if (modifiedTransform) 
		{
			canvasComponent->SetSize(newSize);
		}
	}
	ImGui::EndTable();
}

void InspectorPanel::DrawAudioSourceComponent(AudioSourceComponent* component)
{
	std::vector<const char*> events = App->GetAudio()->GetEventsNames();

	if (ImGui::Button("Play"))
	{
		component->Play();
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		component->Stop(false);
	}

	ImGui::Text("Launch event");
	ImGui::SameLine();

	std::string name = component->GetName();

	if (ImGui::BeginCombo("##audiosourceevent", name.c_str()))
	{
		for (auto i = 0; i < events.size(); i++) 
		{
			if (ImGui::Selectable(events[i]))
			{
				component->SetEventByName(events[i]);
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Event parameters");

	std::vector<int> parameterKeys;
	std::vector<const char*> names;
	std::vector<float> parameterValues;

	component->GetParametersNameAndValue(parameterKeys, names, parameterValues);

	for (auto i = 0; i < parameterKeys.size(); i++)
	{
		const char* name = names[i];
		float value = parameterValues[i];

		float max = 0;
		float min = 0;

		component->GetParametersMaxMin(name, max, min);

		ImGui::Text("%s: ", name);
		ImGui::SameLine();

		std::string str(name);
		std::string tagName = "##" + str;

		if (ImGui::SliderFloat(tagName.c_str(), &value, min, max, "%.0f")) 
		{
			component->UpdateParameterValueByIndex(parameterKeys[i], value);
		}
	}

}

void InspectorPanel::DrawListenerComponent(AudioListenerComponent* component)
{

}
;
void InspectorPanel::DrawButtonComponent(ButtonComponent* imageComponent) 
{
}

void InspectorPanel::DrawSliderComponent(SliderComponent* component)
{
	float value = component->GetValue();

	ImGui::Text("Fill Percent:"); ImGui::SameLine(); ImGui::SliderFloat("##Fill Percent", &value, 0.0f, 1.0f);
	component->SetValue(value);
}

void InspectorPanel::DrawTransform2DComponent(Transform2DComponent* component) 
{
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) 
	{

		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) 
	{
		if (ImGui::Selectable("Reset")) 
		{
			component->ResetTransform();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginTable("transformTable", 4)) 
	{

		bool modifiedTransform = false;
		float3 newPosition = component->GetPosition();
		float3 newRotation = RadToDeg(component->GetRotation());

		const char* labels[2] = { "Position", "Rotation"};
		const char* axisLabels[3] = { "X", "Y", "Z" };
		float3* vectors[2] = { &newPosition, &newRotation };

		for (int i = 0; i < 2; ++i) 
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text(labels[i]);
			ImGui::PopItemWidth();

			for (int j = 0; j < 3; ++j) 
			{
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(axisLabels[j]);
				ImGui::SameLine();
				modifiedTransform = ImGui::DragFloat(axisLabels[j], &(*vectors[i])[j], 0.05f, 0.0f, 0.0f, "%.2f") || modifiedTransform;
				ImGui::PopItemWidth();
			}
			ImGui::PopID();
		}

		if (modifiedTransform)
		{
			component->SetPosition(newPosition);
			component->SetRotation(DegToRad(newRotation));
			modifiedTransform = false;
		}

		float2 newSize = component->GetSize();
		float2 newAnchorMin = component->GetAnchorMin();
		float2 newAnchorMax = component->GetAnchorMax();
		float2 newPivot = component->GetPivot();

		const char* labels2d[4] = { "Size", "Anchor Min", "Anchor Max", "Pivot" };
		const char* axisLabels2d[2] = { "X", "Y" };
		float2* vectors2d[4] = { &newSize, &newAnchorMin, &newAnchorMax, &newPivot };
			
		for (int i = 0; i < 4; ++i) 
		{
			ImGui::PushID(i+2);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text(labels2d[i]);
			ImGui::PopItemWidth();

			for (int j = 0; j < 2; ++j) 
			{
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(axisLabels2d[j]);
				ImGui::SameLine();
				modifiedTransform = ImGui::DragFloat(axisLabels2d[j], &(*vectors2d[i])[j], 0.05f, 0.0f, 0.0f, "%.2f") || modifiedTransform;
				ImGui::PopItemWidth();
			}
			ImGui::PopID();
		}

		if (modifiedTransform)
		{
			component->SetSize(newSize);
			component->SetAnchorMax(newAnchorMax);
			component->SetAnchorMin(newAnchorMin);
			component->SetPivot(newPivot);
		}
	}
	ImGui::EndTable();
	
}

ImColor Float4ToImColor(const float4& color)
{
	ImColor ret = ImColor(color.x, color.y, color.z, color.w);
	return ret;
}

float4 ImColorToFloat4(const float* color)
{
	return float4(color[0], color[1], color[2], color[3]);
}


ImGradient ColorGradientToImGradient(ColorGradient* gradient) {
	ImGradient result;

	const std::map<float, float4>& marks = gradient->GetColorMarks();

	for (const auto& mark : marks) {
		result.addMark(mark.first, Float4ToImColor(mark.second));
	}

	return result;
}


#define FLOAT_TOLERANCE 1e-6f

inline bool approximatelyEqual(float a, float b, float tolerance = FLOAT_TOLERANCE) 
{
	return std::fabs(a - b) < tolerance;
}

bool areMarksEquivalent(const ImGradientMark* a, const std::pair<float, float4> b) {
	if (approximatelyEqual(a->position, b.first)) {
		for (int i = 0; i < 4; ++i) {
			if (!approximatelyEqual(a->color[i], b.second[i])) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool EqualGradients(const ImGradient& editedGradient, ColorGradient* gradient)
{
	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
	auto marks = gradient->GetColorMarks();

	// find if marksEdited has not a mark from the gradient to delete it
	for (const auto& mark : marks) {
		auto it = std::find_if(
			marksEdited.begin(), marksEdited.end(),
			[&](const ImGradientMark* mark1) {
				return areMarksEquivalent(mark1, mark);
			}
		);

		if (it == marksEdited.end())
		{
			return false;
		}
	}
	return true;
}

void findRemovedMarks(const ImGradient& editedGradient, ColorGradient* gradient)
{
	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
	auto marks = gradient->GetColorMarks();

	// find if marksEdited has not a mark from the gradient to delete it
	for (const auto& mark : marks) {
		auto it = std::find_if(marksEdited.begin(), marksEdited.end(),[&](const ImGradientMark* mark1) 
			{
				return areMarksEquivalent(mark1, mark);
			}
		);
		if (it == marksEdited.end()) 
		{
			gradient->RemoveColorGradientMark(mark.first);
		}
	}
}

static void findAddedMarks(const ImGradient& editedGradient, ColorGradient* gradient)
{
	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
	std::map<float, float4> marks = gradient->GetColorMarks();

	// find if markEdit is not in marks to add it
	for (const ImGradientMark* markEdit : marksEdited) {
		auto it = std::find_if(
			marks.begin(), marks.end(),
			[&](const std::pair<float, float4> mark) {
				return areMarksEquivalent(markEdit, mark);
			}
		);
		if (it == marks.end()) 
		{
			gradient->AddColorGradientMark(markEdit->position, float4(markEdit->color));
		}
	}
}

void InspectorPanel::DrawParticleSystemComponent(ParticleSystemComponent* component) const
{
	ImGui::Text("Delay");
	ImGui::SameLine();
	ImGui::DragFloat("##Delay", &(component->mDelay), 0.1f, 0.0f);

	ImGui::Text("Looping");
	ImGui::SameLine(); 
	ImGui::Checkbox("##Looping", &(component->mLooping));
	if (!component->mLooping) 
	{
		ImGui::Text("Duration");
		ImGui::SameLine(); 
		ImGui::DragFloat("##Duration", &(component->mDuration), 0.1f, 0.0f);
	}

	ImGui::Text("Max Particles");
	ImGui::SameLine();
	ImGui::DragInt("##MaxParticles", &(component->mMaxParticles), 0.1f, 0,200);
	ImGui::Text("Emision Rate");
	ImGui::SameLine(); 
	ImGui::DragFloat("##EmisionRate", &(component->mEmissionRate), 0.1f, 0.0f);
	DrawRandomFloat(component->mIsLifetimeRandom, component->mLifetime, component->mMaxLifetime, "Lifetime");

	ImGui::Separator();
	DrawBezierCurve(&(component->mSpeedCurve), "Speed");
	ImGui::Separator();
	ImGui::Text("Stretched Billboard");
	ImGui::SameLine();
	ImGui::Checkbox("##StretchedBillboard", &(component->mStretchedBillboard));
	DrawBezierCurve(&(component->mSizeCurve), "Size");
	ImGui::Separator();
	static const char* items[]{ "Cone","Square","Circle" };
	static int Selecteditem = 0;
	ImGui::Text("Shape");
	ImGui::SameLine();
	bool check = ImGui::Combo("##Shape", &Selecteditem, items, IM_ARRAYSIZE(items));
	if (check)
	{
		component->mShapeType = (EmitterShape::Type)(Selecteditem + 1);
		component->InitEmitterShape();
	}	
	switch(component->mShapeType)
	{
		case EmitterShape::Type::CONE:
			ImGui::Text("Angle");
			ImGui::SameLine();
			ImGui::DragFloat("##Angle", &component->mShape->mShapeAngle, 0.1f, 0.0f);
			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##Radius", &component->mShape->mShapeRadius, 0.1f, 0.0f);
			break;
		case EmitterShape::Type::SQUARE:
			ImGui::Text("Width");
			ImGui::SameLine();
			ImGui::DragFloat2("##Width", &component->mShape->mShapeSize.x, 0.1f, 0.0f);
			break;
		case EmitterShape::Type::CIRCLE:
			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##Radius", &component->mShape->mShapeRadius, 0.1f, 0.0f);
			break;

	}

	DrawBlendTypeSelector(component->mBlendMode, "Blend Type");
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Texture & Tint")) 
	{
		// Drag and drop	
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 70.0);

		ResourceTexture* image = component->GetImage();

		if (image)
		{
			ImTextureID imageID = (void*)(intptr_t)image->GetOpenGLId();
			ImGui::Image(imageID, ImVec2(50, 50));
		}
		else
		{
			ImGui::Text("Drop Image");
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
				Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
				if (resource && (resource->GetType() == Resource::Type::Texture))
				{
					component->SetImage(resource->GetUID());
					component->SetFileName(asset->mName);
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
		if (component->GetFileName() != nullptr)
		{
			ImGui::Text(component->GetFileName());
		}

		if (image)
		{
			ImGui::Text("Width:%dpx", image->GetWidth());
			ImGui::Text("Height:%dpx", image->GetHeight());

		}
		ImGui::Columns(1);

		// Color and alpha
		//float4* color = &component->mColorGradient[0.0f];
		//ImGui::Text("Color:"); ImGui::SameLine(); ImGui::ColorEdit3("", (float*)color);
		//ImGui::Text("Alpha:"); ImGui::SameLine(); ImGui::SliderFloat(" ", &(color->w), 0.0f, 1.0f);

		//::GRADIENT DATA::
		static unsigned int id = 0;
		static ImGradient gradient = ColorGradientToImGradient(component->mColorGradient);
		static ImGradientMark* draggingMark = nullptr;
		static ImGradientMark* selectedMark = nullptr;
		if (!EqualGradients(gradient, component->mColorGradient))
		{
			gradient = ColorGradientToImGradient(component->mColorGradient);
		}
		bool updated = ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

		if (updated) {
			findRemovedMarks(gradient, component->mColorGradient);
			findAddedMarks(gradient, component->mColorGradient);			
		}
	}
}

void InspectorPanel::DrawTextComponent(TextComponent* component)
{
	float3* color = component->GetColor();
	float* alpha = component->GetAlpha();
	std::string* text = component->GetText();
	int* fontSize = component->GetFontSize();
	int* lineSpacing = component->GetLineSpacing();
	int* lineWidth = component->GetLineWidth();

	// Create a buffer for ImGui input
	std::vector<char> buffer(text->begin(), text->end());
	buffer.resize(256); // Adjust buffer size as needed
	buffer.push_back('\0'); // Ensure null-termination

	ImGui::Text("Text:");
	ImGui::SameLine();
	if (ImGui::InputText("##TextInput", buffer.data(), buffer.size())) 
	{
		// Update the std::string if the text was changed
		*text = std::string(buffer.data());
	}

	ImGui::Text("Color:"); ImGui::SameLine(); ImGui::ColorEdit3("##Color", (float*)color);
	ImGui::Text("Alpha:"); ImGui::SameLine(); ImGui::SliderFloat("##Alpha", alpha, 0.0f, 1.0f);

	ImGui::Text("Font Size:"); ImGui::SameLine(); ImGui::DragInt("##Font Size", fontSize);
	ImGui::Text("Line Spacing:"); ImGui::SameLine(); ImGui::DragInt("##Line Space", lineSpacing);
	ImGui::Text("Line Width:"); ImGui::SameLine(); ImGui::DragInt("##Line Width", lineWidth);
	
}

void InspectorPanel::DrawBoxColliderComponent(BoxColliderComponent* component)
{
	if (ImGui::BeginTable("transformTable", 4))
	{
		bool isModified = false;
		float3 newCenter = component->GetCenter();
		float3 newSize = component->GetSize();

		const char* labels[2] = { "Center", "Size" };
		const char* axisLabels[3] = { "X", "Y", "Z" };
		float3* vectors[2] = { &newCenter, &newSize };

		for (int i = 0; i < 2; ++i)
		{
			ImGui::PushID(i);
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text(labels[i]);
			ImGui::PopItemWidth();

			for (int j = 0; j < 3; ++j)
			{
				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(axisLabels[j]);
				ImGui::SameLine();
				isModified = ImGui::DragFloat(axisLabels[j], &(*vectors[i])[j], 0.05f, 0.0f, 0.0f, "%.2f") || isModified;
				ImGui::PopItemWidth();
			}
			ImGui::PopID();

			if (isModified)
			{
				component->SetCenter(newCenter);
				component->SetSize(newSize);
			}
		}
	}
	ImGui::EndTable();

	const char* colliderTypeItems[] = { "Dynamic", "Static", "Kinematic", "Trigger" };
	ColliderType colliderType = component->GetColliderType();
	const char* colliderCurrent = colliderTypeItems[static_cast<int>(colliderType)];
	ImGui::Text("Collider Mode");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##coliderMode", colliderCurrent)) 
	{
		for (int n = 0; n < IM_ARRAYSIZE(colliderTypeItems); ++n) 
		{
			if (ImGui::Selectable(colliderTypeItems[n])) 
			{
				colliderType = ColliderType(n);
				component->SetColliderType(colliderType);
			}
		}
		ImGui::EndCombo();
	}

	bool freezeRotation = component->GetFreezeRotation();
	if (ImGui::Checkbox("Freeze rotation", &freezeRotation))
	{
		component->SetFreezeRotation(freezeRotation);
	}
}

void InspectorPanel::DrawTrailComponent(TrailComponent* component) const
{
	ImGui::Text("Fixed Direction");
	ImGui::SameLine();
	ImGui::Checkbox("##FixedDirection", &(component->mTrail->mFixedDirection));

	if (component->mTrail->mFixedDirection) 
	{
		ImGui::Text("Trail Direction");
		ImGui::SameLine();
		ImGui::DragFloat3("##TrailDirection", component->mTrail->mDirection.ptr());
	}

	ImGui::Text("Minimum distance");
	ImGui::SameLine();
	ImGui::DragFloat("##MinDistance", &(component->mMinDistance), 1.0f, 0.0f);

	ImGui::Text("Lifetime");
	ImGui::SameLine();
	ImGui::DragFloat("##Lifetime", &(component->mTrail->mMaxLifeTime), 1.0f, 0.0f);

	ImGui::Separator();
	DrawBezierCurve(&(component->mTrail->mWidth), "Width");
	
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Texture & Tint"))
	{
		// Drag and drop	
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 70.0);

		ResourceTexture* image = component->mTrail->mImage;

		if (image)
		{
			ImTextureID imageID = (void*)(intptr_t)image->GetOpenGLId();
			ImGui::Image(imageID, ImVec2(50, 50));
		}
		else
		{
			ImGui::Text("Drop Image");
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
			{
				AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
				Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
				if (resource && (resource->GetType() == Resource::Type::Texture))
				{
					component->SetImage(resource->GetUID());
					component->SetFileName(asset->mName);
				}
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
		if (component->GetFileName() != nullptr)
		{
			ImGui::Text(component->GetFileName());
		}

		if (image)
		{
			ImGui::Text("Width:%dpx", image->GetWidth());
			ImGui::Text("Height:%dpx", image->GetHeight());

		}
		ImGui::Columns(1);

		//::GRADIENT DATA::
		static unsigned int id = 0;
		static ImGradient gradient = ColorGradientToImGradient(&component->mTrail->mGradient);
		static ImGradientMark* draggingMark = nullptr;
		static ImGradientMark* selectedMark = nullptr;
		if (!EqualGradients(gradient, &component->mTrail->mGradient))
		{
			gradient = ColorGradientToImGradient(&component->mTrail->mGradient);
		}
		bool updated = ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

		if (updated) 
		{
			findRemovedMarks(gradient, &component->mTrail->mGradient);
			findAddedMarks(gradient, &component->mTrail->mGradient);
		}
	}
}

void InspectorPanel::DrawBezierCurve(BezierCurve* curve, const char* cLabel) const
{
	std::string label = cLabel;
	ImGui::Text(cLabel);
	std::string initial = "Initial " + label;
	DrawRandomFloat(curve->mIsValueRandom, curve->mValue, curve->mMaxValue, initial.c_str());

	ImGui::Text("%s Curved", cLabel);
	ImGui::SameLine();
	std::string asCurve = "##" + label + " Curved";
	ImGui::Checkbox(asCurve.c_str(), &(curve->mIsCurve));
	if (curve->mIsCurve)
	{
		//static float points[5] = { curve->mCurve[0],
		//	curve->mCurve[1],
		//	curve->mCurve[2],
		//	curve->mCurve[3] };
		ImGui::Text("%s Growing Factor", cLabel);
		ImGui::SameLine();
		std::string growing = "##" + label + " Growing Factor";
		ImGui::DragFloat(growing.c_str(), &curve->mFactor, 1.0f, 0.0f);
		//ImGui::Text("Point 1");
		//ImGui::SameLine();
		//ImGui::SliderFloat2("##Point 1", points, 0, 1, "%.3f", 1.0f);
		//ImGui::Text("Point 2");
		//ImGui::SameLine();
		//ImGui::SliderFloat2("##Point 2", &points[2], 0, 1, "%.3f", 1.0f);

		//std::string presets = label + " Presets";

		if (ImGui::Curve("Das editor", ImVec2(400, 200), 10, curve->mPoints))
		{
			// curve changed
		}
	}
}

void InspectorPanel::DrawRandomFloat(bool& isRand, float& minV, float& maxV, const char* cLabel) const
{
	std::string label = cLabel;
	ImGui::Text("%s Rand", cLabel);
	ImGui::SameLine();
	std::string asCurve = "##" + label + "Rand";
	ImGui::Checkbox(asCurve.c_str(), &isRand);
	ImGui::SameLine();
	float itemWidth = ImGui::GetWindowWidth() / 4.0f; // Por ejemplo, un cuarto del ancho de la ventana
	if (!isRand)
	{
		std::string min = "##Min " + label;
		ImGui::PushItemWidth(itemWidth*2); // Establece el ancho para el DragFloat
		ImGui::DragFloat(min.c_str(), &minV);
		ImGui::PopItemWidth(); // Restaura el ancho original
	}
	else
	{
		ImGui::Text("Min");
		std::string min = "##Min " + label;
		ImGui::SameLine();
		ImGui::PushItemWidth(itemWidth); // Establece el ancho para el DragFloat
		ImGui::DragFloat(min.c_str(), &minV);
		ImGui::PopItemWidth(); // Restaura el ancho original

		ImGui::SameLine();
		ImGui::Text("Max");
		ImGui::SameLine();
		std::string max = "##Max " + label;
		ImGui::PushItemWidth(itemWidth); // Establece el ancho para el DragFloat
		ImGui::DragFloat(max.c_str(), &maxV);
		ImGui::PopItemWidth(); // Restaura el ancho original

	}
}

void InspectorPanel::DrawBlendTypeSelector(int& type, const char* cLabel) const
{
	std::string label = cLabel;
	static const char* items[]{ 
		"TRANSPARENCY", 
		"ADDITIVE", 
		"MULTIPLICATIVE", 
		"SUBTRACTIVE", 
		"PREMULTIPLIED_ALPHA", 
		"SCREEN"
	};
	int selected = static_cast<int>(type);
	ImGui::Separator();
	ImGui::Text(cLabel);
	ImGui::SameLine();
	std::string labelID = "##" + label;
	bool check = ImGui::Combo(labelID.c_str(), &selected, items, IM_ARRAYSIZE(items));
	if (check)
	{
		type = static_cast<BlendMode>(selected);
	}
}