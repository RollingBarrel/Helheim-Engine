#include "HierarchyPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

HierarchyPanel::HierarchyPanel() : Panel(HIERARCHYPANEL, true) {}

void HierarchyPanel::Draw(int windowFlags)
{
	GameObject* root = App->GetScene()->GetRoot();
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	ImGui::BeginChild("##tree");
	DrawTree(root, App->GetScene()->GetSelectedGameObject()->GetID());
	ImGui::EndChild();
	OnRightClickNode(root);
	DragAndDropTarget(root);
	ImGui::End();
}

void HierarchyPanel::OnRightClickNode(GameObject* node) {
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("OptionsGO", ImGuiPopupFlags_NoOpenOverExistingPopup);
	}
	if (ImGui::BeginPopup("OptionsGO")) {
		if (ImGui::Selectable("Create GameObject")) {
			GameObject* gameObject = new GameObject(node);
			node->AddChild(gameObject);
			App->GetScene()->SetSelectedObject(gameObject);
		}

		if (!node->mIsRoot) {
			if (ImGui::Selectable("Duplicate")) {
				GameObject* gameObject = new GameObject(*node);
				//mParent->AddChild(gameObject);
				App->GetScene()->AddGameObjectToDuplicate(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
			}

			if (ImGui::Selectable("Delete")) {
				App->GetScene()->AddGameObjectToDelete(node);
				App->GetScene()->SetSelectedObject(App->GetScene()->GetRoot());
			}
		}
		ImGui::EndPopup();
	}
}

void HierarchyPanel::DrawTree(GameObject* node, const int selected)
{
	ImGui::PushID(node->GetID());
	bool nodeOpen = true;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	if (!node->mIsRoot) {
		ImGui::InvisibleButton("##", ImVec2(-1, 5)); //This can be changed to modify the spacing between elements
		DragAndDropTarget(node, true);
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (node->mID == selected)
			baseFlags |= ImGuiTreeNodeFlags_Selected;
		if (node->mChildren.size() == 0) {
			baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)node->mID, baseFlags, node->mName.c_str()) && (node->mChildren.size() > 0);
		ImGui::PopStyleVar();
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(node);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(node);
		}
		OnRightClickNode(node);
		DragAndDropSource(node);
	}
	else {
		nodeOpen = ImGui::CollapsingHeader(node->mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		ImGui::PopStyleVar();
	}
	DragAndDropTarget(node);
	if (nodeOpen) {
		for (auto child : node->mChildren) {
			DrawTree(child, selected);
		}

		if (!node->mIsRoot) {
			ImGui::TreePop();
		}
	}

	//if (node->mIsRoot) {
	//	ImGui::InvisibleButton("##", ImVec2(-1, -1)); //This can be changed to modify the spacing between elements
	//	DragAndDropTarget(node);
	//}
	ImGui::PopID();
}

void HierarchyPanel::DragAndDropSource(GameObject* source)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("_TREENODE", source, sizeof(*source));

		ImGui::Text(source->mName.c_str());
		ImGui::EndDragDropSource();
	}
}

void HierarchyPanel::DragAndDropTarget(GameObject* target, bool reorder) {
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			bool isParent = false;
			const GameObject* movedObject = (const GameObject*)payload->Data;

			GameObject* parent = target->mParent;

			while (parent != nullptr) {
				if (parent->mID == movedObject->mID) {
					isParent = true;
				}
				parent = parent->mParent;
			}

			if (!isParent) {
				GameObject* pMovedObject = movedObject->mParent->RemoveChild(movedObject->GetID());
				if (reorder) { target->mParent->AddChild(pMovedObject, target->mID); }
				else { target->AddChild(pMovedObject); }
			}

		}
		ImGui::EndDragDropTarget();
	}
}
