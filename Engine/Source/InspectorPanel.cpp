#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include <MathFunc.h>

bool InspectorPanel::mSame_component_popup = false;

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	GameObject* focusedObject = App->GetScene()->GetSelectedGameObject();
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
	
	if (mSame_component_popup)
	{
		ShowSameComponentPopup();
	}

	ImGui::End();
	ImGui::PopID();
}

void InspectorPanel::DrawTransform(GameObject* object) {
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

			float3 newRotation = RadToDeg(object->mEulerRotation);
			float3 newPosition = object->mPosition;
			float3 newScale = object->mScale;

			ImGui::TableNextRow();
			ImGui::PushID(object->mID);
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Position");
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &newPosition.x, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetPosition(newPosition);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &newPosition.y, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetPosition(newPosition);
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &newPosition.z, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetPosition(newPosition);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();


			ImGui::TableNextRow();
			ImGui::PushID(object->mID + 1);
			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Rotation");
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &newRotation.x, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetRotation(DegToRad(newRotation));
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &newRotation.y, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetRotation(DegToRad(newRotation));
			}
			ImGui::PopItemWidth();

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &newRotation.z, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetRotation(DegToRad(newRotation));
			}
			ImGui::PopItemWidth();

			ImGui::PopID();


			ImGui::TableNextRow();
			ImGui::PushID(object->mID + 2);

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::Text("Scale");

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("X");
			ImGui::SameLine();
			if (ImGui::DragFloat("##X", &newScale.x, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetScale(newScale);
			}

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Y");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Y", &newScale.y, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetScale(newScale);
			}

			ImGui::TableNextColumn();
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Z");
			ImGui::SameLine();
			if (ImGui::DragFloat("##Z", &newScale.z, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				object->SetScale(newScale);
			}
			ImGui::PopItemWidth();

			ImGui::PopID();

		}
		ImGui::EndTable();
	}

}
//void InspectorPanel::DrawTransform(GameObject* object) {
//
//
//	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
//	ImGui::SameLine(ImGui::GetItemRectSize().x - 50.0f);
//	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
//	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
//	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
//	if (ImGui::SmallButton("Config##transform")) {
//		ImGui::OpenPopup("TransformOptions");
//	}
//	if (ImGui::BeginPopup("TransformOptions")) {
//		if (ImGui::Selectable("Reset")) {
//			object->ResetTransform();
//		}
//		ImGui::EndPopup();
//	}
//
//	ImGui::PopStyleColor(3);
//	if (headerOpen) {
//		bool modifiedTransform = false;
//		if (ImGui::BeginTable("transformTable", 2)) {
//			float3 position = object->mLocalTransformMatrix.TranslatePart();
//			float3 rotation = RadToDeg(object->mLocalTransformMatrix.ToEulerXYZ());
//			float3 scale = object->mLocalTransformMatrix.GetScale();
//			const char* labels[3] = { "Position", "Rotation", "Scale" };
//			float3* vectors[3] = { &(position), &(rotation), &(scale) };
//
//			for (int i = 0; i < 3; ++i) {
//				ImGui::PushID(i);
//				ImGui::TableNextRow();
//				ImGui::TableSetColumnIndex(0);
//				ImGui::Text(labels[i]);
//				ImGui::TableSetColumnIndex(1);
//				ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
//
//				modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &vectors[i]->x);
//				ImGui::SameLine();
//				modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &vectors[i]->y);
//				ImGui::SameLine();
//				modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &vectors[i]->z);
//				ImGui::PopItemWidth();
//				ImGui::PopID();
//			}
//
//			if (modifiedTransform) {
//				object->RecalculateMatrices();
//			}
//		}
//		ImGui::EndTable();
//	}
//
//}

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
				mSame_component_popup = true;
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

	ImGui::Begin("Can't add the same component multiple times!", &mSame_component_popup,
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
		mSame_component_popup = false;
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

void InspectorPanel::DrawComponents(GameObject* object) {
	for (auto component : object->mComponents) {
		ImGui::PushID(component->mID);
		bool isOpen = ImGui::CollapsingHeader(component->mName, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
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
}

void InspectorPanel::DrawTestComponent(TestComponent* component) {
	ImGui::Text("Demo Text");
	ImGui::Text("Demo Text 2 ");
}

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent* component) {
	ImGui::Text("Model: Cube.obj (TEST)");
	ImGui::Text("Material: DefaultMaterial (TEST)");
	ImGui::Text("Shader: StandardShader (TEST)");
	bool a = component->ShouldDraw();
	if(ImGui::Checkbox("Draw bounding box:", &a))
		component->SetShouldDraw(a);
}
