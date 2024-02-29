#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "HierarchyPanel.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "CameraComponent.h"
#include "ImporterMaterial.h"
#include "MathFunc.h"

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	HierarchyPanel* hierarchyPanel = (HierarchyPanel *) App->GetEditor()->GetPanel(HIERARCHYPANEL);
	GameObject* focusedObject = hierarchyPanel->GetFocusedObject();
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
		ImGui::PushID(focusedObject->mID);
		ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
		focusedObject->mName = nameArray;
		ImGui::PopID();

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

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) {
		if (ImGui::Selectable("Reset")) {
			object->ResetTransform();
		}
		ImGui::EndPopup();
	}

	if (headerOpen) {
		if (ImGui::BeginTable("transformTable", 4)) {
			//ImGui::TableSetupColumn("columns", 0 , -FLT_MIN);

			bool modifiedTransform = false;
			float3 newPosition = object->mPosition;
			float3 newRotation = RadToDeg(object->mEulerRotation);
			float3 newScale = object->mScale;

			const char* labels[3] = { "Position", "Rotation", "Scale" };
			const char* axisLabels[3] = { "X", "Y", "Z" };
			float3* vectors[3] = { &newPosition, &newRotation, &newScale };

			for (int i = 0; i < 3; ++i) {
				ImGui::PushID(i);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Text(labels[i]);
				ImGui::PopItemWidth();

				for (int j = 0; j < 3; ++j) {
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

			if (modifiedTransform) {
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

	ImGui::SetCursorPosX(posX);

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		for (unsigned int i = 0; i <= static_cast<unsigned int>(ComponentType::NONE) - 1; ++i) {
			ComponentType currentComponent = static_cast<ComponentType>(i);
			if (ImGui::MenuItem(Component::GetNameFromType(currentComponent))) {
				mComponent = object->GetComponent(currentComponent);
				if (!mComponent)
				{
					object->CreateComponent(currentComponent);
				}
				else {
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

	if (ImGui::Button("Cancel", ImVec2(120, 25))) {
		mSameComponentPopup = false;
	}

	ImGui::End();
}

/****************** COMPONENTS ********************/

void InspectorPanel::RightClickPopup(Component* component) {

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(std::to_string(component->mID).c_str());
	}

	if (ImGui::BeginPopup(std::to_string(component->mID).c_str())) {
		if (ImGui::MenuItem("Delete Component")) {
			component->mOwner->AddComponentToDelete(component);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Reset Component")) {
			component->Reset();
			ImGui::CloseCurrentPopup();
		}

		switch (component->GetType()) {
			case ComponentType::TEST: {
				if (ImGui::MenuItem("Custom Test Component Option")) {
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Custom Test Component Option 2")) {
					ImGui::CloseCurrentPopup();
				}
				break;
			}
			case ComponentType::MESHRENDERER: {
				if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
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
		LOG("Droped payload");
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COMPONENT"))
		{
			Component* movedComponent = (Component*)payload->Data;
			if (target != nullptr ? movedComponent->GetID() != target->GetID() : true) {
				Component* pMovedComponent = object->RemoveComponent(movedComponent);
				object->AddComponent(pMovedComponent, target);
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopStyleVar();
}

void InspectorPanel::DrawComponents(GameObject* object) {
	for (auto component : object->mComponents) {
		ImGui::PushID(component->mID);
		DragAndDropTarget(object, component);

		bool isOpen = ImGui::CollapsingHeader(Component::GetNameFromType(component->GetType()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

		//checkbox for enable/disable
		ImGui::Checkbox("Enable", &component->mIsEnabled);

		DragAndDropSource(component);
		RightClickPopup(component);
		if (isOpen) {
			switch (component->GetType()) {
				case ComponentType::MESHRENDERER: {
					DrawMeshRendererComponent(reinterpret_cast<MeshRendererComponent*>(component));
					break;
				}
				case ComponentType::POINTLIGHT: {
					DrawPointLightComponent(reinterpret_cast<PointLightComponent*>(component));
					break;
				}
				case ComponentType::SPOTLIGHT: {
					DrawSpotLightComponent(reinterpret_cast<SpotLightComponent*>(component));
					break;
				}
				case ComponentType::CAMERA: {
					DrawCameraComponent(reinterpret_cast<CameraComponent*>(component));
					break;
				}
				case ComponentType::SCRIPT: {
					DrawScriptComponent(reinterpret_cast<ScriptComponent*>(component));
					break;
				}
				case ComponentType::TEST: {
					DrawTestComponent(reinterpret_cast<TestComponent*>(component));
					break;
				}
			}
		}
		ImGui::PopID();
	}
	DragAndDropTarget(object, nullptr);
}

void InspectorPanel::DrawTestComponent(TestComponent* component) {
	ImGui::Text("Demo Text");
	ImGui::Text("Demo Text 2 ");
}

void InspectorPanel::DrawPointLightComponent(PointLightComponent* component) {
	const float* pCol = component->GetColor();
	float col[3] = { pCol[0], pCol[1] , pCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 1.0f, 0.0f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	{
		component->SetRadius(radius);
	}
	ImGui::Checkbox("Debug draw", &component->debugDraw);
}

void InspectorPanel::DrawSpotLightComponent(SpotLightComponent* component) {
	const float* sCol = component->GetColor();
	float col[3] = { sCol[0], sCol[1] , sCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	const float* sDir = component->GetDirection();
	float dir[3] = { sDir[0], sDir[1] , sDir[2] };
	if (ImGui::DragFloat3("Direction", dir, 0.05f, -1.f, 1.f))
	{
		component->SetDirection(dir);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 1.0f, 0.0f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	{
		component->SetRadius(radius);
	}
	float innerAngle = RadToDeg(component->GetInnerAngle());
	float outerAngle = RadToDeg(component->GetOuterAngle());
	if (ImGui::DragFloat("Inner angle", &innerAngle, 1.0f, 0.0f, outerAngle))
	{
		component->SetInnerAngle(DegToRad(innerAngle));
	}
	if (ImGui::DragFloat("Outer angle", &outerAngle, 1.0f, innerAngle, 90.f))
	{
		component->SetOuterAngle(DegToRad(outerAngle));
	}
	ImGui::Checkbox("Debug draw", &component->debugDraw);
}

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent* component) {

	ImGui::SeparatorText("Material");

	MaterialVariables(component);

	ImGui::Text(" ");
	bool shouldDraw = component->ShouldDraw();
	if (ImGui::Checkbox("Draw bounding box:", &shouldDraw)) {
		component->SetShouldDraw(shouldDraw);
	}
}

void InspectorPanel::MaterialVariables(MeshRendererComponent* renderComponent)
{
	ResourceMaterial* material = const_cast<ResourceMaterial*>(renderComponent->GetMaterial());


	ImGui::Checkbox("Enable Diffuse map", &material->mEnableDiffuseTexture);
	ImGui::Checkbox("Enable Specular map", &material->mEnableSpecularGlossinessTexture);
	ImGui::Checkbox("Enable Shininess map", &material->mEnableShinessMap);
	ImGui::Checkbox("Enable Normal map", &material->mEnableNormalMap);

	if (!material->mEnableDiffuseTexture)
	{
		ImGui::ColorPicker3("Diffuse", material->mDiffuseFactor.ptr());
	}
	if (!material->mEnableSpecularGlossinessTexture)
	{
		ImGui::ColorPicker3("Specular", material->mSpecularFactor.ptr());
	}
	if (!material->mEnableShinessMap)
	{
		ImGui::DragFloat("Shininess", &material->mGlossinessFactor, 0.05f, 0.0f, 10000.0f, "%.2f");
	}
}

void InspectorPanel::DrawCameraComponent(CameraComponent* component)
{
	ImGui::SeparatorText("Camera");

	float Near = component->GetNearPlane();
	float* NearBar = &Near;
	const char* NearLabel = "NearPlane";

	float Far = component->GetFarPlane();
	float* FarBar = &Far;
	const char* FarLabel = "FarPlane";

	float FOV = RadToDeg(component->GetVerticicalFOV());
	float* FOVBar = &FOV;
	const char* FOVLabel = "FOV";

	bool modifiedValue = false;

	ImGui::PushID(NearLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Near Plane");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(NearLabel, &(*NearBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	ImGui::PushID(FarLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Far Plane");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(FarLabel, &(*FarBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	ImGui::PushID(FOVLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("FOV");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(FOVLabel, &(*FOVBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	if (modifiedValue)
	{
		component->SetNearPlane(Near);
		component->SetFarPlane(Far);
		component->SetVerticicalFOV(DegToRad(FOV));
	}

	//ImGui::Checkbox("Enable Diffuse map", &(new bool(true)));
	// Is culling
}

void InspectorPanel::DrawScriptComponent(ScriptComponent* component)
{
	ImGui::Text("SCRIPTCOMP");
}
