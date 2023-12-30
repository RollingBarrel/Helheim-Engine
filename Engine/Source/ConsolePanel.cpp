#include "ConsolePanel.h"

#include "imgui.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "Math/float4x4.h"

ConsolePanel::ConsolePanel() : Panel("Console##", true)
{
}

ConsolePanel::~ConsolePanel()
{
}

void ConsolePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Text("Mira como beben los peces en el rio");
		float4x4 viewproj = App->GetCamera()->GetViewProjMatrix();
		ImGui::InputFloat4("Camera 0", &viewproj[0][0]);
		ImGui::InputFloat4("Camera 1", &viewproj[1][0]);
		ImGui::InputFloat4("Camera 2", &viewproj[2][0]);
		ImGui::InputFloat4("Camera 3", &viewproj[3][0]);


	}
	ImGui::End();
}
