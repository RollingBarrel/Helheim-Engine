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
		for (const char* logs : log) {
			ImGui::TextUnformatted(logs);
		}
	}	
	ImGui::End();
}

void ConsolePanel::SetLog(const char* logBuffer)
{
    if (log.size() >= VECTOR_LENGH) {
        delete[] log[0];
        log.erase(log.begin());
    }

    size_t len = strlen(logBuffer);
    char* newLog = new char[len + 1];
    memcpy(newLog, logBuffer, len + 1);

    log.push_back(newLog);
}