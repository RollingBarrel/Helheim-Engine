#include "ConsolePanel.h"
#include "imgui.h"
#include "Application.h"
#include "Globals.h"

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
		ImGui::TextUnformatted(GetLogBuffer());
	}
	ImGui::End();
}
