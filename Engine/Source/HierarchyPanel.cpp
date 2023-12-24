#include "HierarchyPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"

HierarchyPanel::HierarchyPanel() : Panel(HIERARCHYPANEL, true) {}

void HierarchyPanel::Draw(int windowFlags)
{
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	OpenRightClickPanel();


	if (ImGui::Button("Create GameObject")) {
		GameObject* gameObject = new GameObject(App->GetScene()->getRoot());
		App->GetScene()->getRoot()->AddChild(gameObject);
		App->GetScene()->SetSelectedObject(gameObject);
	}


	if (ImGui::Button("Duplicate GameObject")) {
		GameObject* gameObject = new GameObject(*(App->GetScene()->getSelectedGameObject()));
		App->GetScene()->getRoot()->AddChild(gameObject);
		App->GetScene()->SetSelectedObject(gameObject);
	}
	App->GetScene()->DrawHierarchy();

	ImGui::End();
}

void HierarchyPanel::OpenRightClickPanel()
{
	if(ImGui::IsWindowHovered()) {
		if (ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup("Options");
		}
	}
	//if (ImGui::IsItemClicked(1)) {
	//	ImGui::OpenPopup("Options");
	//}
	if (ImGui::BeginPopup("Options")) {
		ImGui::Button("Create GameObject");
		ImGui::EndPopup();
	}
	


}
