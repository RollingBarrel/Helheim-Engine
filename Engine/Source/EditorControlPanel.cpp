#include "EditorControlPanel.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"


EditorControlPanel::EditorControlPanel() : Panel(EDITORCONTROLPANEL, true)
{
}

EditorControlPanel::~EditorControlPanel()
{
}

void EditorControlPanel::Draw(int windowFlags)
{
	ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(150, 60), ImGuiCond_Once);
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (ImGui::Button("Translate", ImVec2(40, 40))) {
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotate", ImVec2(40, 40))) {
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Scale", ImVec2(40, 40))) {
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Local", ImVec2(40, 40))) {
		mCurrentGuizmoMode = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::Button("World", ImVec2(40, 40))) {
		mCurrentGuizmoMode = ImGuizmo::WORLD;
	}

	//Change the Guizmo operation using W,E & R keywords and the coordinate mode with G
	if (ImGui::IsKeyPressed(ImGuiKey_W))
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	if (ImGui::IsKeyPressed(ImGuiKey_G))
		mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	
	ImGui::End();
}
