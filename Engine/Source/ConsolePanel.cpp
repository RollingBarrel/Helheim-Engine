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
		string logBuffer = GetLogBuffer(); //Get the log content
		const char* logChar = logBuffer.c_str(); //Convert log string to char

		ImGui::TextUnformatted(logChar); //Gets log.cpp char result of all LOG messages. 
	}
	ImGui::End();
}
