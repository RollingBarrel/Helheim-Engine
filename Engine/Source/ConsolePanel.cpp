#include "ConsolePanel.h"

#include "imgui.h"
#include "Application.h"


ConsolePanel::ConsolePanel() : Panel(CONSOLEPANEL, true)
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



	}
	ImGui::End();
}
