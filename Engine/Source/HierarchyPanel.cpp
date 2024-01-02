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
	App->GetScene()->DrawHierarchy();

	ImGui::End();
}

void HierarchyPanel::OpenRightClickPanel()
{
	static bool optionsPopUp = true;
	if(ImGui::IsWindowHovered()) {
		if (ImGui::GetIO().MouseClicked[1])
		{
			ImGui::OpenPopup("Options");
			optionsPopUp = true;
		}
	}
	if (optionsPopUp) {
		if (ImGui::BeginPopup("Options")) {
			if (ImGui::Button("Create GameObject")) {
				GameObject* gameObject = new GameObject(App->GetScene()->GetSelectedGameObject());
				App->GetScene()->GetSelectedGameObject()->AddChild(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
				optionsPopUp = false;
			}
			if (!(App->GetScene()->GetSelectedGameObject()->IsRoot())) {
				if (ImGui::Button("Duplicate GameObject")) {
					GameObject* gameObject = new GameObject(*(App->GetScene()->GetSelectedGameObject()));
					App->GetScene()->GetSelectedGameObject()->GetParent()->AddChild(gameObject);
					App->GetScene()->SetSelectedObject(gameObject);
					optionsPopUp = false;
				}
			}
			if (!(App->GetScene()->GetSelectedGameObject()->IsRoot())) {
				if (ImGui::Button("Delete GameObject")) {
					App->GetScene()->AddGameObjectToDelete(App->GetScene()->GetSelectedGameObject()->GetID());
					optionsPopUp = false;
				}
			}

			ImGui::EndPopup();
		}
	}


}
