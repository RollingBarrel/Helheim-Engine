#include "InspectorPanel.h"

#include "ImBezier.h"
#include "imgui.h"
#include "ImColorGradient.h"
#include "ImGuiFileDialog.h"

#include "EngineApp.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "HierarchyPanel.h"
#include "SettingsPanel.h"
#include "ProjectPanel.h"
#include "ModuleCamera.h"
#include "ModuleScriptManager.h"
#include "ModuleAudio.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"
#include "ModuleUI.h"
#include "GameObject.h"

#include "MeshRendererComponent.h"
#include "ModuleEngineResource.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "CameraComponent.h"
#include "AIAGentComponent.h"
#include "ImageComponent.h"
#include "MaskComponent.h"
#include "CanvasComponent.h"
#include "ButtonComponent.h"
#include "AudioSourceComponent.h"
#include "Transform2DComponent.h"
#include "ParticleSystemComponent.h"
#include "TextComponent.h"
#include "TrailComponent.h"
#include "BoxColliderComponent.h"
#include "NavMeshObstacleComponent.h"
#include "AnimationComponent.h"
#include "SliderComponent.h"
#include "DecalComponent.h"

#include "ImporterMaterial.h"
#include "MathFunc.h"
#include "Script.h"
#include "AnimationController.h"
#include "BezierCurve.h"
#include "AudioUnit.h"
#include "TrailComponent.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceAnimation.h"
#include "ResourceModel.h"
#include "ResourceStateMachine.h"

#include "IconsFontAwesome6.h"


#include "AnimationStateMachine.h"
#include "AnimationSMPanel.h"
#include "SaveLoadMaterial.h"

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	HierarchyPanel* hierarchyPanel = (HierarchyPanel*)EngineApp->GetEditor()->GetPanel(HIERARCHYPANEL);
	SettingsPanel* settingsPanel =  reinterpret_cast<SettingsPanel*>(EngineApp->GetEditor()->GetPanel(SETTINGSPANEL));
	
	GameObject* focusedObject = hierarchyPanel->GetFocusedObject();

	if (mLockedGameObject != nullptr) 
	{
		focusedObject = mLockedGameObject;
	}

	if (focusedObject == nullptr) return;

	char nameArray[100];
	strcpy_s(nameArray, focusedObject->mName.c_str());
	bool enabled = focusedObject->IsEnabled();
	ImGui::PushID(focusedObject->mUid);
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
		ImGui::PushID(focusedObject->mUid);
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


		if (ImGui::BeginCombo("##tags", focusedObject->GetTag().c_str()))
		{
			for (unsigned int i = 0; i < settingsPanel->GetTags().size(); i++)
			{
				ImGui::SetNextItemAllowOverlap();
				if (ImGui::Selectable(settingsPanel->GetTags()[i].c_str(), false, 0, ImVec2(100.0f, 20.0f)))
				{
					focusedObject->SetTag(settingsPanel->GetTags()[i]);

					AnimationComponent* animComp = reinterpret_cast<AnimationComponent*>(focusedObject->GetComponent(ComponentType::ANIMATION));
					if (animComp)
					{
						animComp->ReloadGameObjects();
					}
				}
				
				
				if (i >= NUM_ENGINE_TAGS)
				{
					ImGui::SameLine();
					ImGui::PushID(i);
					if (ImGui::Button(ICON_FA_TRASH_CAN))
					{
						settingsPanel->DeleteTag(settingsPanel->GetTags()[i].c_str());
					}
					ImGui::PopID();
				}

			}
			ImGui::Separator();
			mTagsLayersPopUp = ImGui::Button("Add Tag...");
			ImGui::EndCombo();
		}

		if (focusedObject->mPrefabId != 0) {
			ImGui::Text("From Prefab");
			ImGui::Checkbox("Override Prefab", &focusedObject->mIsPrefabOverride);
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

	if (mTagsLayersPopUp)
	{
		ImGui::OpenPopup("Tags&Layers");
		ShowTagsLayerPopUp();
	}

	if (settingsPanel->mDeleteTagPopup)
	{
		ImGui::OpenPopup("DeleteTag");
		settingsPanel->ShowDeleteTagsPopup();
	}

	ImGui::End();
	ImGui::PopID();
}

void InspectorPanel::ShowTagsLayerPopUp()
{
	SettingsPanel* settingsPanel = reinterpret_cast<SettingsPanel*>(EngineApp->GetEditor()->GetPanel(SETTINGSPANEL));

	if(ImGui::BeginPopup("Tags&Layers"))
	{
		static char tmp[280] = "";
		ImGui::InputText("New Tag", tmp, IM_ARRAYSIZE(tmp), ImGuiInputTextFlags_AlwaysOverwrite);
		if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::Button("Save"))
		{
			if (strcmp(tmp, "") != 0)
			{
				settingsPanel->AddTag(tmp);
			}
			memset(tmp, 0, 280);
			mTagsLayersPopUp = false;
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel"))
		{
			mTagsLayersPopUp = false;
			memset(tmp, 0, 280);
		}

		ImGui::EndPopup();
	}
}

void InspectorPanel::DrawTransform(GameObject* object)
{
	ImGui::PushID(object->mUid);
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
			float3 newPosition = object->GetLocalPosition();
			float3 newRotation = RadToDeg(object->GetLocalEulerAngles());
			float3 newScale = object->GetLocalScale();

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
				object->SetLocalPosition(newPosition);
				object->SetLocalRotation(DegToRad(newRotation));
				object->SetLocalScale(newScale);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopID();
}

void InspectorPanel::AddComponentButton(GameObject* object) 
{
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

//void InspectorPanel::DragAndDropSource(Component* component) {
//	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
//	{
//		ImGui::SetDragDropPayload("_COMPONENT", component, sizeof(*component));
//
//		ImGui::Text(Component::GetNameFromType(component->GetType()));
//		ImGui::EndDragDropSource();
//	}
//}
//
//void InspectorPanel::DragAndDropTarget(GameObject* object, Component* target) {
//	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
//	ImGui::InvisibleButton("##", ImVec2(-1, 5));
//	if (ImGui::BeginDragDropTarget())
//	{
//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COMPONENT"))
//		{
//			Component* movedComponent = (Component*)payload->Data;
//			if (target != nullptr ? movedComponent->GetID() != target->GetID() : true) 
//			{
//				Component* pMovedComponent = object->RemoveComponent(movedComponent);
//				object->AddComponent(pMovedComponent, target);
//			}
//		}
//		ImGui::EndDragDropTarget();
//	}
//	ImGui::PopStyleVar();
//}

void InspectorPanel::DrawComponents(GameObject* object) 
{
	for (Component* component : object->mComponents) 
	{
		ImGui::PushID(component->mID);
		//DragAndDropTarget(object, component);

		bool isOpen = ImGui::CollapsingHeader(Component::GetNameFromType(component->GetType()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		RightClickPopup(component);

		bool isEnabled = component->IsEnabled();
		if (ImGui::Checkbox("Enable", &isEnabled)) 
		{
			component->SetEnable(isEnabled);
		}

		//DragAndDropSource(component);
		
		if (isOpen) 
		{
			switch (component->GetType()) 
			{
				case ComponentType::MESHRENDERER:
					DrawMeshRendererComponent(*reinterpret_cast<MeshRendererComponent*>(component));
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
				case ComponentType::IMAGE:
					DrawImageComponent(reinterpret_cast<ImageComponent*>(component));
					break;
				case ComponentType::MASK:
					DrawMaskComponent(reinterpret_cast<MaskComponent*>(component));
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
				case ComponentType::DECAL:
					DrawDecalComponent(reinterpret_cast<DecalComponent*>(component));
					break;
			}
		}
		ImGui::PopID();
	}
	//DragAndDropTarget(object, nullptr);
}

void InspectorPanel::DrawPointLightComponent(PointLightComponent* component) {
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

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent& component) 
{
	static bool createMaterialPopUp = false;

	const ResourceMaterial& rMat = *component.GetResourceMaterial();
	ImGui::SeparatorText("Material");

	//Si el material te name, treurel per imgui !!!
	MaterialVariables(component);

	ImGui::Separator();
	if (rMat.GetName() != nullptr)
	{
		ImGui::Text(rMat.GetName());
		if (ImGui::Button("Save Material"))
		{
			//Just save the resource material
			Importer::Material::SaveMatFile(rMat);
			Importer::Material::Save(&rMat);
		}
	}

	//if (ImGui::Button("Set Material"))
	//{
	// chose a material from the assets material foler and set it
	//}

	if (ImGui::Button("Create New Material"))
	{
		createMaterialPopUp = true;
	}
	if (createMaterialPopUp)
	{
		ImGui::OpenPopup("CreateMaterial");
		if (ImGui::BeginPopup("CreateMaterial"))
		{
			ImGui::Text("Give the new material a name");
			static char userInputName[200] = "";
			ImGui::InputText("New Material Name", userInputName, IM_ARRAYSIZE(userInputName), ImGuiInputTextFlags_AlwaysOverwrite);
			if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::Button("Create"))
			{
				if (strcmp(userInputName, "") != 0)
				{
					std::string assetName = ASSETS_MATERIAL_PATH;
					assetName += userInputName;
					assetName += ".mat";
					if (!App->GetFileSystem()->Exists(assetName.c_str()))
					{
						Importer::Material::SaveMatFile(rMat, userInputName);
						unsigned int newUid = EngineApp->GetEngineResource()->ImportFile(assetName.c_str());
						component.SetMaterial(newUid);
					}
					else
					{
						LOG("ERROR: This material name already exists");
					}
					createMaterialPopUp = false;
				}
				memset(userInputName, 0, 200);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				memset(userInputName, 0, 200);
				createMaterialPopUp = false;
			}
			ImGui::EndPopup();
		}
	}
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

void InspectorPanel::MaterialVariables(const MeshRendererComponent& renderComponent)
{
	ResourceMaterial* material = const_cast<ResourceMaterial*>(renderComponent.GetResourceMaterial());

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

	static float FOV = RadToDeg(component->GetHorizontalFOV());
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
		EngineApp->GetFileSystem()->DiscoverFiles(ASSETS_SCRIPT_PATH, ".h", scriptNames);
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
						App->GetScriptManager()->RemoveGameObjectFromMap(gameObject);
						*gameObject = *(GameObject**)payload->Data;
						App->GetScriptManager()->AddGameObjectToMap(gameObject);
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

	bool loop = component->GetLoop();
	//bool play = false;


	if (component->GetAnimationUid() != 0)
	{

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

		if (ImGui::Button("Open state machine panel"))
		{
			AnimationSMPanel* panel = reinterpret_cast<AnimationSMPanel*>(EngineApp->GetEditor()->GetPanel(ANIMATIONSMPANEL));
			panel->SetStateMachine(component->GetStateMachine());
			panel->Open();
		}

		//Draw selectable state machines
		std::vector<std::string> assets;
		GetStateMachineAssets(component, false, assets);
		const char* smName = component->GetStateMachine()->GetName().c_str();
		ImGui::Text("Select default state machine:");
		if (ImGui::BeginCombo("##DefaultSM", smName))
		{

			for (int n = 0; n < assets.size(); n++)
			{
				bool is_selected = (smName == assets[n]);
				if (ImGui::Selectable(assets[n].c_str(), is_selected))
				{
					smName = assets[n].c_str();
					std::string path = std::string("Assets/StateMachines/" + assets[n] + ".smbin");
					ResourceStateMachine* newSM = reinterpret_cast<ResourceStateMachine*>(EngineApp->GetResource()->RequestResource(path.c_str()));
					component->SetStateMachine(newSM->GetStateMachine());
					component->SetSMUID(newSM->GetUID());
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		
		if (component->HasSpine())
		{
			ImGui::Text("Select spine state machine:");
			const char* spineSMName = component->GetSpineStateMachine()->GetName().c_str();
			if (ImGui::BeginCombo("##SpineSM", spineSMName))
			{

				for (int n = 0; n < assets.size(); n++)
				{
					bool is_selected = (spineSMName == assets[n]);
					if (ImGui::Selectable(assets[n].c_str(), is_selected))
					{
						spineSMName = assets[n].c_str();
						std::string path = std::string("Assets/StateMachines/" + assets[n] + ".smbin");
						ResourceStateMachine* newSM = reinterpret_cast<ResourceStateMachine*>(EngineApp->GetResource()->RequestResource(path.c_str()));
						component->SetSpineStateMachine(newSM->GetStateMachine());
						component->SetSpineSMUID(newSM->GetUID());

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
}

void InspectorPanel::GetStateMachineAssets(AnimationComponent* component, bool isSpine, std::vector<std::string>& names)
{
	
	const char* currentItem = component->GetStateMachine()->GetName().c_str();
	if (isSpine)
	{
		currentItem = component->GetSpineStateMachine()->GetName().c_str();

	}
	EngineApp->GetFileSystem()->DiscoverFiles("Assets/StateMachines", ".smbin", names);
	for (int i = 0; i < names.size(); ++i)
	{

		size_t slashPos = names[i].find_last_of('/');
		if (slashPos != std::string::npos)
		{

			names[i].erase(0, slashPos + 1);
		}

		size_t dotPos = names[i].find_first_of('.');
		if (dotPos != std::string::npos)
		{

			names[i].erase(dotPos);
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

	// Maskable checkbox
	bool maskable = imageComponent->GetIsMaskable();
	if (ImGui::Checkbox("Maskable", &maskable))
	{
		imageComponent->SetMaskable(maskable);
	}
}

void InspectorPanel::DrawMaskComponent(MaskComponent* component)
{
	if (component->GetMask() == nullptr)
		ImGui::Text("No image component attached");
	else
	{
		ImGui::Text("Has Image attached");

		bool drawMask = component->GetDrawMask();
		if (ImGui::Checkbox("Show Mask Graphic", &drawMask))
		{
			component->SetDrawMask(drawMask);
		}

		const char* maskingModes[] = { "Normal", "Inverse" };
		int currentMode = static_cast<int>(component->GetMaskingMode());

		if (ImGui::BeginCombo("Masking Mode", maskingModes[currentMode]))
		{
			for (int i = 0; i < IM_ARRAYSIZE(maskingModes); i++)
			{
				bool isSelected = (currentMode == i);
				if (ImGui::Selectable(maskingModes[i], isSelected))
				{
					component->SetMaskingMode(static_cast<MaskComponent::MaskingMode>(i));
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}
}


void InspectorPanel::DrawCanvasComponent(CanvasComponent* canvasComponent) 
{
	const char* renderModes[] = { "World Space", "Screen Space", "Billboard mode", "World axis billboard"};
	static int selectedRenderMode = static_cast<int>(canvasComponent->GetRenderSpace());

	ImGui::Text("Render Mode");
	ImGui::SameLine();

	if (ImGui::Combo("##RenderModeCombo", &selectedRenderMode, renderModes, IM_ARRAYSIZE(renderModes))) 
	{
		switch (selectedRenderMode)
		{
		case -1: break;
		case 0: canvasComponent->SetRenderSpace(RenderSpace::World);
			break;
		case 1: canvasComponent->SetRenderSpace(RenderSpace::Screen);
			break;
		case 2: canvasComponent->SetRenderSpace(RenderSpace::Billboard);
			break;
		case 3: canvasComponent->SetRenderSpace(RenderSpace::WorldAxisBillboard);
			break;
		}
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

void InspectorPanel::DrawAudioSourceComponent(AudioSourceComponent* component) {
	// List event and add
	std::vector<const char*> events = App->GetAudio()->GetEventsNames();

	ImGui::Text("Launch event");
	ImGui::SameLine();

	static std::string selectedEventName = "";
	if (ImGui::BeginCombo("##audiosourceevent", selectedEventName.c_str()))
	{
		for (auto i = 0; i < events.size(); i++)
		{
			if (ImGui::Selectable(events[i]))
			{
				selectedEventName = events[i];
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Add Event"))
	{
		component->AddNewAudioByName(selectedEventName.c_str());
	}

	// Set event one by one
	ImGui::Separator();
	int counter = 1;
	for (auto audioUnit : component->GetAudios())
	{
		ImGui::Text("%i : %s",counter, audioUnit->GetName().c_str());
		ImGui::PushID(audioUnit);
		if (ImGui::Button("Play"))
		{
			audioUnit->Play();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			audioUnit->Release();
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			component->RemoveAudio(audioUnit);
			ImGui::PopID();
			return;
		}
		
		ImGui::SameLine();
		ImGui::Separator();
		

		std::vector<int> parameterKeys;
		std::vector<const char*> names;
		std::vector<float> parameterValues;

		if (audioUnit->GetPreviewID() == -1)
		{
			ImGui::Text("Press 'Play' to view parameters");
		}
		else 
		{
			audioUnit->GetParametersNameAndValue(parameterKeys, names, parameterValues);
			ImGui::Text("Num Event parameters: %i", parameterKeys.size());
		}
	
		for (auto i = 0; i < parameterKeys.size(); i++)
		{
			const char* paramName = names[i];
			float value = parameterValues[i];

			float max = 0;
			float min = 0;
			audioUnit->GetParametersMaxMin(paramName, max, min);

			ImGui::Text("%s: ", paramName);
			ImGui::SameLine();

			std::string str(paramName);
			std::string tagName = "##" + str;

			if (ImGui::SliderFloat(tagName.c_str(), &value, min, max, "%.0f"))
			{
				//audioUnit->UpdateParameterValueByIndex(parameterKeys[i], value);
				audioUnit->UpdateParameterValueByName(paramName, value);
			}
		}
		ImGui::PopID();
		counter++;
		ImGui::Separator();
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

//ImColor Float4ToImColor(const float4& color)
//{
//	ImColor ret = ImColor(color.x, color.y, color.z, color.w);
//	return ret;
//}
//
//float4 ImColorToFloat4(const float* color)
//{
//	return float4(color[0], color[1], color[2], color[3]);
//}
//
//ImGradient ColorGradientToImGradient(ColorGradient* gradient) {
//	ImGradient result;
//
//	const std::map<float, float4>& marks = gradient->GetColorMarks();
//
//	for (const auto& mark : marks) {
//		result.addMark(mark.first, Float4ToImColor(mark.second));
//	}
//
//	return result;
//}
//
//
//#define FLOAT_TOLERANCE 1e-6f
//
//inline bool approximatelyEqual(float a, float b, float tolerance = FLOAT_TOLERANCE) 
//{
//	return std::fabs(a - b) < tolerance;
//}
//
//bool areMarksEquivalent(const ImGradientMark* a, const std::pair<float, float4> b) {
//	if (approximatelyEqual(a->position, b.first)) {
//		for (int i = 0; i < 4; ++i) {
//			if (!approximatelyEqual(a->color[i], b.second[i])) {
//				return false;
//			}
//		}
//		return true;
//	}
//	return false;
//}
//
//bool EqualGradients(const ImGradient& editedGradient, ColorGradient* gradient)
//{
//	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
//	auto marks = gradient->GetColorMarks();
//
//	// find if marksEdited has not a mark from the gradient to delete it
//	for (const auto& mark : marks) {
//		auto it = std::find_if(
//			marksEdited.begin(), marksEdited.end(),
//			[&](const ImGradientMark* mark1) {
//				return areMarksEquivalent(mark1, mark);
//			}
//		);
//
//		if (it == marksEdited.end())
//		{
//			return false;
//		}
//	}
//	return true;
//}
//
//void findRemovedMarks(const ImGradient& editedGradient, ColorGradient* gradient)
//{
//	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
//	auto marks = gradient->GetColorMarks();
//
//	// find if marksEdited has not a mark from the gradient to delete it
//	for (const auto& mark : marks) {
//		auto it = std::find_if(marksEdited.begin(), marksEdited.end(),[&](const ImGradientMark* mark1) 
//			{
//				return areMarksEquivalent(mark1, mark);
//			}
//		);
//		if (it == marksEdited.end()) 
//		{
//			gradient->RemoveColorGradientMark(mark.first);
//		}
//	}
//}
//
//static void findAddedMarks(const ImGradient& editedGradient, ColorGradient* gradient)
//{
//	const std::list<ImGradientMark*>& marksEdited = editedGradient.getMarks();
//	std::map<float, float4> marks = gradient->GetColorMarks();
//
//	// find if markEdit is not in marks to add it
//	for (const ImGradientMark* markEdit : marksEdited) {
//		auto it = std::find_if(
//			marks.begin(), marks.end(),
//			[&](const std::pair<float, float4> mark) {
//				return areMarksEquivalent(markEdit, mark);
//			}
//		);
//		if (it == marks.end()) 
//		{
//			gradient->AddColorGradientMark(markEdit->position, float4(markEdit->color));
//		}
//	}
//}

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
	DrawRandomFloat(component->mLifetime, "Lifetime");

	ImGui::Separator();
	DrawBezierCurve(&(component->mSpeedCurve), "Speed");
	ImGui::Separator();
	ImGui::Text("Stretched Billboard");
	ImGui::SameLine();
	ImGui::Checkbox("##StretchedBillboard", &(component->mStretchedBillboard));
	ImGui::Text("Stretched Ratio");
	ImGui::SameLine();
	ImGui::DragFloat("##StretchedRatio", &(component->mStretchedRatio), 0.1f, 0.0f);

	DrawBezierCurve(&(component->mSizeCurve), "Size");
	ImGui::Separator();
	static const char* items[]{ "Cone","Box","Sphere"};
	static int Selecteditem = 0;
	ImGui::Text("Shape");
	ImGui::SameLine();
	bool check = ImGui::Combo("##Shape", &Selecteditem, items, IM_ARRAYSIZE(items));
	if (check)
	{
		component->mShapeType = (ParticleSystemComponent::EmitterType)(Selecteditem + 1);
	}	
	switch(component->mShapeType)
	{
		case ParticleSystemComponent::EmitterType::CONE:
			ImGui::Text("Angle");
			ImGui::SameLine();
			ImGui::DragFloat("##Angle", &component->mShapeAngle, 0.1f, 0.0f);
			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##Radius", &component->mShapeRadius, 0.1f, 0.0f);
			break;
		case ParticleSystemComponent::EmitterType::BOX:
			ImGui::Text("Width");
			ImGui::SameLine();
			ImGui::DragFloat3("##Width", &component->mShapeSize.x, 0.1f, 0.0f);
			ImGui::Text("Invers Dir");
			ImGui::SameLine();
			ImGui::Checkbox("##Invers Dir", &(component->mShapeInverseDir));

			break;
		case ParticleSystemComponent::EmitterType::SPHERE:
			ImGui::Text("Radius");
			ImGui::SameLine();
			ImGui::DragFloat("##Radius", &component->mShapeRadius, 0.1f, 0.0f);
			ImGui::Text("Invers Dir");
			ImGui::SameLine();
			ImGui::Checkbox("##Invers Dir", &(component->mShapeInverseDir));

			break;
	}
	ImGui::Text("Rand Dir");
	ImGui::SameLine();
	ImGui::Checkbox("##FixedDirection", &(component->mIsShapeAngleRand));
	if (component->mIsShapeAngleRand) 
	{
		ImGui::SameLine();
		ImGui::DragFloat("##RandDirection", &component->mShapeRandAngle, 0.1f, 0.0f);
	}

	DrawBlendTypeSelector(component->mBlendMode, "Blend Type");
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Texture & Tint")) 
	{
		// Drag and drop	
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 70.0);

		const ResourceTexture* image = component->GetImage();

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
		static float draggingMark = -1.0f;
		static float selectedMark = -1.0f;
		bool updated = ImGui::GradientEditor(component->mColorGradient, draggingMark, selectedMark);
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
	if (ImGui::InputTextMultiline("##TextInput", buffer.data(), buffer.size())) 
	{
		// Update the std::string if the text was changed
		*text = std::string(buffer.data());
	}

	ImGui::Text("Color:"); ImGui::SameLine(); ImGui::ColorEdit3("##Color", (float*)color);
	ImGui::Text("Alpha:"); ImGui::SameLine(); ImGui::SliderFloat("##Alpha", alpha, 0.0f, 1.0f);
	
	ImGui::Text("Alignment:");
	if (ImGui::Button("Left"))
	{
		component->SetAlignment(TextAlignment::LEFT);
	}
	ImGui::SameLine(); 
	if (ImGui::Button("Center"))
	{
		component->SetAlignment(TextAlignment::CENTER);
	}
	ImGui::SameLine(); 
	if (ImGui::Button("Right"))
	{
		component->SetAlignment(TextAlignment::RIGHT);
	}

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
	ImGui::Checkbox("##FixedDirection", &(component->mFixedDirection));

	if (component->mFixedDirection) 
	{
		ImGui::Text("Trail Direction");
		ImGui::SameLine();
		ImGui::DragFloat3("##TrailDirection", component->mDirection.ptr());
	}

	ImGui::Text("Minimum distance");
	ImGui::SameLine();
	ImGui::DragFloat("##MinDistance", &(component->mMinDistance), 1.0f, 0.0f);

	ImGui::Text("Lifetime");
	ImGui::SameLine();
	ImGui::DragFloat("##Lifetime", &(component->mMaxLifeTime), 1.0f, 0.0f);

	ImGui::Separator();
	DrawBezierCurve(&(component->mWidth), "Width");
	
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Texture & Tint"))
	{
		// Drag and drop	
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 70.0);

		ResourceTexture* image = component->mImage;

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

		ImGui::Text("UV Scroll");
		ImGui::SameLine();
		ImGui::Checkbox("##IsUVScrolling", &(component->mIsUVScrolling));
		if (component->mIsUVScrolling) 
		{
			ImGui::SameLine();
			ImGui::DragFloat("##UVScroll", &(component->mUVScroll), 1.0f, 0.0f);
		}

		static float draggingMark = -1.0f;
		static float selectedMark = -1.0f;
		bool updated = ImGui::GradientEditor(component->mGradient, draggingMark, selectedMark);
	}
}

void InspectorPanel::DrawDecalComponent(DecalComponent* component)
{

	unsigned int imageSize = 50;
	const unsigned int numTextures = 4;

	ResourceTexture** textures[numTextures] = { &component->mDiffuseTexture , &component->mSpecularTexture , &component->mNormalTexture, &component->mEmisiveTexture };
	std::string* fileNames[numTextures] = { &component->mDiffuseName, &component->mSpecularName , &component->mNormalName, &component->mEmisiveName };
	const char* names[numTextures] = { "Diffuse Texture", "Specular Texture", "Normal Map", "Emisive Texture"};
	float4* colors[numTextures] = { &component->mDiffuseColor, nullptr, nullptr, &component->mEmisiveColor };
	

	ImGui::SliderFloat("fade", &component->mFade, 0.0f, 1.0f);

	ImGui::Checkbox("Sprite Sheet", &component->mIsSpriteSheet);
	if (component->mIsSpriteSheet)
	{
		// Play/Pause/Stop buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 1.0f, 0.6f, 1.0f)); // Pale green
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text
		if (ImGui::Button("Play"))
		{
			component->Play();
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 0.6f, 1.0f)); // Pale yellow
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Pause"))
		{
			component->Pause();
		}
		ImGui::PopStyleColor(2);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.6f, 0.6f, 1.0f)); // Pale red
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("Stop"))
		{
			component->Stop();
		}
		ImGui::PopStyleColor(2);

		
		// Columns and rows selector
		if (ImGui::InputInt("Row", &component->mDefaultRow))
		{
			component->mDefaultRow = Clamp(static_cast<float>(component->mDefaultRow), 0.0f, static_cast<float>(component->mRows - 1));
			component->mCurrentRow = component->mDefaultRow;
		}
		if (ImGui::InputInt("Column", &component->mDefaultColumn))
		{
			component->mDefaultColumn = Clamp(static_cast<float>(component->mDefaultColumn), 0.0f, static_cast<float>(component->mColumns - 1));
			component->mCurrentColumn = component->mDefaultColumn;
		}

		if (ImGui::InputInt("Rows", &component->mRows))
		{
			component->mRows = (component->mRows < 1) ? 1 : component->mRows;
		}
		if (ImGui::InputInt("Columns", &component->mColumns))
		{
			component->mColumns = (component->mColumns < 1) ? 1 : component->mColumns;
		}
		
		// Reproduction speed slider
		ImGui::Text("Speed (FPS):"); ImGui::SameLine();
		ImGui::PushItemWidth(100);
		ImGui::SliderInt("##Reproduction Speed", &component->mFPS, 1, 100);
		ImGui::PopItemWidth();


		// Display the spritesheet image
		ResourceTexture* image = nullptr;

		for (unsigned int i = 0; i < numTextures; ++i)
		{
			if (*textures[i])
			{
				image = *textures[i];
				break;
			}
		}

		if (image)
		{
			ImVec2 imageSize(175, 175);
			ImGui::Image((void*)(intptr_t)image->GetOpenGLId(), imageSize);
		
			// Draw lines to divide the image into columns and rows
			ImVec2 imagePos = ImGui::GetItemRectMin();
			float columnWidth = imageSize.x / component->mColumns;
			float rowHeight = imageSize.y / component->mRows;
			for (int i = 1; i < component->mColumns; ++i)
			{
				ImVec2 start(imagePos.x + i * columnWidth, imagePos.y);
				ImVec2 end(imagePos.x + i * columnWidth, imagePos.y + imageSize.y);
				ImGui::GetWindowDrawList()->AddLine(start, end, IM_COL32(255, 255, 255, 255));
			}
			for (int i = 1; i < component->mRows; ++i)
			{
				ImVec2 start(imagePos.x, imagePos.y + i * rowHeight);
				ImVec2 end(imagePos.x + imageSize.x, imagePos.y + i * rowHeight);
				ImGui::GetWindowDrawList()->AddLine(start, end, IM_COL32(255, 255, 255, 255));
			}
		
			// Display a list of the slices
			if (ImGui::CollapsingHeader("Slices", ImGuiTreeNodeFlags_None))
			{
				float sliceWidth = 1.0f / component->mColumns;
				float sliceHeight = 1.0f / component->mRows;
				ImVec2 sliceSize(50, 50);
		
				for (int row = 0; row < component->mRows; ++row)
				{
					for (int col = 0; col < component->mColumns; ++col)
					{
						// Calculate the texture coordinates for the slice
						ImVec2 uv0(col * sliceWidth, row * sliceHeight);
						ImVec2 uv1((col + 1) * sliceWidth, (row + 1) * sliceHeight);
		
						ImGui::Image((void*)(intptr_t)image->GetOpenGLId(), sliceSize, uv0, uv1);
		
						if (col < component->mColumns - 1)
						{
							ImGui::SameLine();
							ImGui::Text(" ");
							ImGui::SameLine();
						}
					}
					if (row < component->mRows - 1)
					{
						ImGui::NewLine();
					}
				}
			}
		}
	}

	if (ImGui::BeginTable("1", 2, ImGuiTableFlags_BordersInner))
	{

		for (int i = 0; i < 4; ++i)
		{
			ImGui::BeginGroup();
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::Text(names[i]);
			if (*textures[i])
			{
				ImTextureID imageID = (void*)(intptr_t)(*textures[i])->GetOpenGLId();

				if (colors[i])
				{
					ImGui::Image((void*)(intptr_t)imageID, ImVec2(imageSize, imageSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(colors[i]->x, colors[i]->y, colors[i]->z, colors[i]->w));
				}
				else
				{
					ImGui::Image((void*)(intptr_t)imageID, ImVec2(imageSize, imageSize));
				}
				
		
				if (colors[i])
				{
					ImGui::ColorEdit4(names[i], colors[i]->ptr(), ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
				}

				ImGui::TableNextColumn();
				if (!(*fileNames[i]).empty())
				{
					ImGui::Text((*fileNames[i]).c_str());
				}
				if ((*textures[i]))
				{
					ImGui::Text("Width:%dpx", (*textures[i])->GetWidth());
					ImGui::Text("Height:%dpx", (*textures[i])->GetHeight());
				}
				ImGui::PushID(i);

				

				if (ImGui::Button(ICON_FA_TRASH_CAN))
				{
					App->GetResource()->ReleaseResource((*textures[i])->GetUID());
					*textures[i] = nullptr;
				}
				ImGui::PopID();
			}
			else
			{

				ImGui::Image(0, ImVec2(imageSize, imageSize));
				ImGui::TableNextColumn();
				ImGui::Dummy(ImVec2(0.0f, 20.0f));
				ImGui::Text("Drop Texture");
			}
			ImGui::EndGroup();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCENE"))
				{
					AssetDisplay* asset = reinterpret_cast<AssetDisplay*>(payload->Data);
					Resource* resource = EngineApp->GetResource()->RequestResource(asset->mPath);
					if (resource && (resource->GetType() == Resource::Type::Texture))
					{
						(*textures[i]) = reinterpret_cast<ResourceTexture*>(resource);
						(*textures[i])->GenerateMipmaps();
						(*fileNames[i]) = asset->mName;
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Dummy(ImVec2(1,5.0f));
		}

		ImGui::EndTable();
	}
}

void InspectorPanel::DrawBezierCurve(BezierCurve* curve, const char* cLabel) const
{
	std::string label = cLabel;
	ImGui::Text(cLabel);
	std::string initial = "Initial " + label;
	DrawRandomFloat(curve->mValue, initial.c_str());

	ImGui::Text("%s Curved", cLabel);
	ImGui::SameLine();
	std::string asCurve = "##" + label + " Curved";
	ImGui::Checkbox(asCurve.c_str(), &(curve->mIsCurve));
	if (curve->mIsCurve)
	{
		if (ImGui::Curve("Das editor", ImVec2(400, 200), 10, curve->mPoints))
		{
			// curve changed
		}
	}
}

void InspectorPanel::DrawRandomFloat(RandomFloat& value, const char* cLabel) const
{
	std::string label = cLabel;
	ImGui::Text("%s Rand", cLabel);
	ImGui::SameLine();
	std::string asCurve = "##" + label + "Rand";
	ImGui::Checkbox(asCurve.c_str(), &value.mIsRand);
	ImGui::SameLine();
	float itemWidth = ImGui::GetWindowWidth() / 4.0f; // Por ejemplo, un cuarto del ancho de la ventana
	if (!value.mIsRand)
	{
		std::string min = "##Min " + label;
		ImGui::PushItemWidth(itemWidth*2); // Establece el ancho para el DragFloat
		ImGui::DragFloat(min.c_str(), &value.mMin);
		ImGui::PopItemWidth(); // Restaura el ancho original
	}
	else
	{
		ImGui::Text("Min");
		std::string min = "##Min " + label;
		ImGui::SameLine();
		ImGui::PushItemWidth(itemWidth); // Establece el ancho para el DragFloat
		ImGui::DragFloat(min.c_str(), &value.mMin);
		ImGui::PopItemWidth(); // Restaura el ancho original

		ImGui::SameLine();
		ImGui::Text("Max");
		ImGui::SameLine();
		std::string max = "##Max " + label;
		ImGui::PushItemWidth(itemWidth); // Establece el ancho para el DragFloat
		ImGui::DragFloat(max.c_str(), &value.mMax);
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
		type = selected;
	}
}