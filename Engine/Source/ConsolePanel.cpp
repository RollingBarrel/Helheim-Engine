#include "ConsolePanel.h"
#include "imgui.h"
#include "Globals.h"

ConsolePanel::ConsolePanel() : Panel(CONSOLEPANEL, true)
{
    mLogs = new ImGuiTextBuffer();
}

ConsolePanel::~ConsolePanel()
{
    delete mLogs;
}

void ConsolePanel::Draw(int windowFlags)
{
    windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;

    static bool autoScroll = true;

	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
			ImGui::TextUnformatted(mLogs->begin(), mLogs->end());

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
            

	}	
	ImGui::End();
}

void ConsolePanel::AddLog(const char* log)
{
    mLogs->appendf(log);
}