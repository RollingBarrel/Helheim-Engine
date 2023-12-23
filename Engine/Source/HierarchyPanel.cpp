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