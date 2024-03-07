#include "EditorControlPanel.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "HierarchyPanel.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "GameObject.h"

#include "IconsFontAwesome6.h"


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

	ImVec2 buttonSize(30, 30);

	if (ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_ROTATE, buttonSize)) {
		mCurrentGuizmoOperation = ImGuizmo::SCALE;
	}
	ImGui::SameLine();
	const char* buttonLabel = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ICON_FA_CUBE " Local" : ICON_FA_GLOBE " World";
	if (ImGui::Button(buttonLabel, ImVec2(80,30))) {
		mCurrentGuizmoMode = (mCurrentGuizmoMode == ImGuizmo::LOCAL) ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
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
