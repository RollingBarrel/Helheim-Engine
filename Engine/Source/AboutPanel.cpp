#include "AboutPanel.h"

#include "imgui.h"

AboutPanel::AboutPanel() : Panel("About##", true)
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
