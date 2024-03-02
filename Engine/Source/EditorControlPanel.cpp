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
	ImGui::SetNextWindowSize(ImVec2(200, 80), ImGuiCond_Once);
	windowFlags |= ImGuiWindowFlags_NoTitleBar;
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	ImVec2 buttonSize(50, 50);

	ImGui::BeginGroup();
	if (ImGui::Button("Translate", buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Rotate", buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button("Scale", buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	}
	ImGui::EndGroup();

	ImGui::SameLine();
	ImGui::BeginGroup();
	const char* buttonLabel = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? "Local" : "World";
	if (ImGui::Button(buttonLabel, buttonSize)) {
		mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	}
	ImGui::EndGroup();

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
