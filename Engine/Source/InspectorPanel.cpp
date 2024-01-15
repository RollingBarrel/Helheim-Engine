#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"

bool InspectorPanel::mSame_component_window = false;

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

	if (!focusedObject->IsRoot()) {
		ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
		focusedObject->mName = nameArray;
		DrawTransform(focusedObject);
		DrawComponents(focusedObject);

		ImGui::Separator();
		AddComponentButton(focusedObject);
	}
	
	if (mSame_component_window) ShowSameComponentWindow();

	ImGui::End();
	ImGui::PopID();
}

void InspectorPanel::DrawTransform(GameObject* object) {
	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
	ImGui::SameLine(ImGui::GetItemRectSize().x - 50.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
	if (ImGui::SmallButton("Config##transform")) {
		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) {
		if (ImGui::Selectable("Reset")) {
			object->ResetTransform();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(3);
	if (headerOpen) {
		bool modifiedTransform = false;
		if (ImGui::BeginTable("transformTable", 2)) {
			const char* labels[3] = { "Position", "Rotation", "Scale" };
			float3* vectors[3] = { &(object->mPosition), &(object->mRotation), &(object->mScale) };

			for (int i = 0; i < 3; ++i) {
				ImGui::PushID(i);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(labels[i]);
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);

				modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &vectors[i]->x);
				ImGui::SameLine();
				modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &vectors[i]->y);
				ImGui::SameLine();
				modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &vectors[i]->z);
				ImGui::PopItemWidth();
				ImGui::PopID();
			}

			if (modifiedTransform) {
				object->RecalculateMatrices();
			}
		}
		ImGui::EndTable();
	}

}

void InspectorPanel::AddComponentButton(GameObject* object) {
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	ImGui::SetCursorPosX(posX);

	bool hasMeshRendererComponent = CheckComponent(object, ComponentType::MESHRENDERER);

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (ImGui::MenuItem("Mesh Renderer")) {
			if (!hasMeshRendererComponent)
			{
				object->CreateComponent(ComponentType::MESHRENDERER);
			} else {
				mSame_component_window = true;
			}
		}
		if (ImGui::MenuItem("Test")) {
			object->CreateComponent(ComponentType::TEST);
		}
		ImGui::EndPopup();
	}
}

bool InspectorPanel::CheckComponent(GameObject* object, ComponentType type) {
	for (Component* component : object->mComponents) {
		if (component->GetType() == type) {
			return true;
			break;
		}
	}
	return false;
}

void InspectorPanel::ShowSameComponentWindow() 
{
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImVec2 centerPos = ImVec2(mainViewport->Pos.x + mainViewport->Size.x * 0.5f,
		mainViewport->Pos.y + mainViewport->Size.y * 0.5f);

	ImGui::SetNextWindowPos(centerPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(380, 115), ImGuiCond_Appearing);

	ImGui::Begin("Can't add the same component multiple times!", &mSame_component_window,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("This component can't be added because GameObject ");
	ImGui::Text("already contains the same component");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImVec2 buttonSize(120, 25);
	ImVec2 buttonPos((ImGui::GetWindowSize().x - buttonSize.x) * 0.5f, 90);

	if (ImGui::Button("Cancel", buttonSize)) {
		mSame_component_window = false;
	}

	ImGui::SetCursorPos(buttonPos);
	ImGui::End();
}

/****************** COMPONENTS ********************/

void InspectorPanel::RightClickPopup(Component* component) {

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(component->mPopupID);
	}

	if (ImGui::BeginPopup(component->mPopupID)) {
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
	ImGui::Text("Model: Cube.obj (TEST)");
	ImGui::Text("Material: DefaultMaterial (TEST)");
	ImGui::Text("Shader: StandardShader (TEST)");
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