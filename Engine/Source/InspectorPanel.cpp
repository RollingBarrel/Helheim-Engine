#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleEditor.h"
#include "HierarchyPanel.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "ImporterMaterial.h"

#include <MathFunc.h>

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	HierarchyPanel* hierarchyPanel = (HierarchyPanel *) App->GetEditor()->GetPanel(HIERARCHYPANEL);
	GameObject* focusedObject = hierarchyPanel->GetFocusedObject();
	if (focusedObject == nullptr) return;

	char nameArray[100];
	strcpy_s(nameArray, focusedObject->mName.c_str());
	ImGui::PushID(focusedObject->mID);
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (!focusedObject->IsRoot()) 
	{
		ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
		focusedObject->mName = nameArray;
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
			float3 newRotation = RadToDeg(object->mEulerRotation);
			float3 newPosition = object->mPosition;
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
		if (ImGui::MenuItem("Mesh Renderer")) {
			mComponent = object->GetComponent(ComponentType::MESHRENDERER);
			if (!mComponent)
			{
				object->CreateComponent(ComponentType::MESHRENDERER);
			} else {
				mSameComponentPopup = true;
			}
		}
		if (ImGui::MenuItem("Test")) {
			object->CreateComponent(ComponentType::TEST);
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

	ImGui::Text("The component %s can't be added because", mComponent->mName);
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

		ImGui::Text(component->mName);
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
		bool isOpen = ImGui::CollapsingHeader(component->mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		DragAndDropSource(component);
		RightClickPopup(component);
		if (isOpen) {
			switch (component->GetType()) {
				case ComponentType::MESHRENDERER: {
					MeshRendererComponent* downCast = dynamic_cast<MeshRendererComponent*>(component);
					assert(downCast != nullptr);
					DrawMeshRendererComponent(downCast);
					break;
				}
				case ComponentType::TEST: {
					TestComponent* downCast = dynamic_cast<TestComponent*>(component);
					assert(downCast != nullptr);
					DrawTestComponent(downCast);
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

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent* component) {
	ImGui::SeparatorText("Model ");
	int selectedOption = 0;

	if (ImGui::BeginCombo("Model", nullptr)) {
		// List of mesh options
		const char* meshOptions[] = {
			"Clock",
			"DollHouse",
			"Drawers",
			"Duck",
			"Firetruck",
			"Floor",
			"Hearse",
			"Player",
			"SpinningTop",
			"Robot",
			"Wall",
			"ZomBunny"
		};

		for (int i = 0; i < IM_ARRAYSIZE(meshOptions); i++) {
			bool isSelected = (i == selectedOption);
			if (ImGui::Selectable(meshOptions[i], isSelected)) {
				selectedOption = i;
			}

			// If the option is selected, set the focus on it
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
	
	ImGui::SeparatorText("Material");
	ImGui::Text("WATING FOR FILE SYSTEM �? (TEST)");
	ImGui::Text("CREATE NAME VARIABLE?");

	ImGui::Text(" ");

	if (ImGui::BeginTable("materialTable", 4))
	{
		float* ambient = component->ambientColor;

		ImGui::TableNextRow();
		ImGui::PushID("ambient");
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Ambient");
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &ambient[0], 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			component->ambientColor[0] = ambient[0];
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &ambient[1], 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			component->ambientColor[1] = ambient[1];
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &ambient[2], 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			component->ambientColor[2] = ambient[2];
		}
		ImGui::PopID();
	}
	ImGui::EndTable();

	//std::vector<Mesh*> meshes = component->getMeshes();
	//
	//for (auto i = 0; i < meshes.size(); i++) {
	//	ImGui::PushID(i);
	//	ResourceMaterial* material = meshes[i]->GetMaterial();
	//	if (ImGui::CollapsingHeader(("Mesh " + std::to_string(i)).c_str())) {
	//
	//		MaterialVariables(material, i);
	//	}
	//	ImGui::PopID();
	//}

	ImGui::SeparatorText("Shaders ");
	ImGui::Text("Vertex: "); ImGui::SameLine(); ImGui::Text(App->GetProgram()->GetVertexShader());
	ImGui::Text("Fragment: "); ImGui::SameLine(); ImGui::Text(App->GetProgram()->GetFragmentShader());

	ImGui::Text(" ");
	bool shouldDraw = component->ShouldDraw();
	ImGui::Checkbox("Draw bounding box:", &shouldDraw);
}

void InspectorPanel::MaterialVariables(MeshRendererComponent* renderComponent, int i)
{
	ResourceMaterial* material = const_cast<ResourceMaterial*>(renderComponent->GetMaterial());

	if (ImGui::BeginTable("materialTable", 4))
	{

		float4 diffuse = material->GetDiffuseFactor();
		float3 specular = material->GetSpecularFactor();
		float shininess = material->GetGlossinessFactor();

		ImGui::TableNextRow();
		ImGui::PushID(i + "diff");
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Diffuse");
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &diffuse.x, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetDiffuseFactor(float4(diffuse.x, diffuse.y, diffuse.z, material->GetDiffuseFactor().w));
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &diffuse.y, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetDiffuseFactor(float4(diffuse.x, diffuse.y, diffuse.z, material->GetDiffuseFactor().w));
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &diffuse.z, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetDiffuseFactor(float4(diffuse.x, diffuse.y, diffuse.z, material->GetDiffuseFactor().w));
		}
		ImGui::PopID();

		ImGui::TableNextRow();
		ImGui::PushID(i + "spec");
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Specular");
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &specular.x, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetSpecularFactor(float3(specular.x, specular.y, specular.z));
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &specular.y, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetSpecularFactor(float3(specular.x, specular.y, specular.z));
		}

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &specular.z, 0.05f, 0.0f, 1.0f, "%.2f"))
		{
			material->SetSpecularFactor(float3(specular.x, specular.y, specular.z));
		}
		ImGui::PopID();

		ImGui::TableNextRow();
		ImGui::PushID(i + "shiny");
		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text("Shininess");
		ImGui::PopItemWidth();

		ImGui::TableNextColumn();
		ImGui::PushItemWidth(-FLT_MIN);
		ImGui::Text(" ");
		ImGui::SameLine();
		if (ImGui::DragFloat("##S", &shininess, 0.05f, 0.0f, 10000.0f, "%.2f"))
		{
			material->SetGlossinessFactor(shininess);
		}

		ImGui::PopID();
	}
	ImGui::EndTable();

	ImGui::Text(" ");

	bool hasDiffuse = material->GetEnableDiffuseTexture();
	bool hasSpecular = material->GetEnableSpecularGlossinessTexture();
	bool hasShininess = material->GetEnableShinessMap();

	ImGui::Checkbox("Enable Diffuse map", &hasDiffuse);
	ImGui::Checkbox("Enable Specular map", &hasSpecular);
	ImGui::Checkbox("Enable Shininess map", &hasShininess);

	material->SetEnableDiffuseTexture((int)hasDiffuse);
	material->SetEnableSpecularGlossinessTexture((int)hasSpecular);
	material->SetEnableShinessMap((int)hasShininess);

}
