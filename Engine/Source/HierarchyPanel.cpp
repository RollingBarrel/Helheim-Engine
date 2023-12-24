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
