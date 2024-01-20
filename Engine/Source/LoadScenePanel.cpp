#include "LoadScenePanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"


LoadScenePanel::LoadScenePanel() : Panel(LOADSCENEPANEL, false)
{
}

LoadScenePanel::~LoadScenePanel()
{
}

void LoadScenePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Text("Which file you wish to load?");
		static char fileName[128] = "";
		ImGui::InputText(".json", fileName, IM_ARRAYSIZE(fileName));
		if (ImGui::Button("Load")) {
			// Add logic here to load file
		}
	}
	ImGui::End();
}
