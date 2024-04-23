#include "ConsolePanel.h"
#include "imgui.h"
#include "Application.h"
#include "Globals.h"

ConsolePanel::ConsolePanel() : Panel(CONSOLEPANEL, true)
{
}

ConsolePanel::~ConsolePanel()
{
    for (auto oneLog : log) {
        delete[] oneLog;
    }
}

void ConsolePanel::Draw(int windowFlags)
{
    windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;

    static bool autoScroll = true;

	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		for (const char* logs : log) 
        {
			ImGui::TextUnformatted(logs);
		}

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
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