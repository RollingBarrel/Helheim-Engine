#include "AboutPanel.h"
#include "imgui.h"

AboutPanel::AboutPanel() : Panel(ABOUTPANEL, false)
{
}

AboutPanel::~AboutPanel()
{
}

void AboutPanel::Draw(int windowFlags) 
{
	//ImGui::SetNextWindowSize(ImVec2(300, 200));
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Text("Welcum :)");
	}
	ImGui::End();
}
