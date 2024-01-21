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
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Text("Welcum");
	}
	ImGui::End();
}
