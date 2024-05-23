#pragma once
#include "Panel.h"

#define CONSOLEPANEL "Console##"
#define LOGS_MAX_SIZE 300

struct ImGuiTextBuffer;

class ConsolePanel : public Panel
{
public:
	ConsolePanel();
	~ConsolePanel();

	void Draw(int windowFlags) override;
	void AddLog(const char* log);

private:
	ImGuiTextBuffer* mLogs = nullptr;
};