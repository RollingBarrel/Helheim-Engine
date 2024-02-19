#pragma once
#include "Panel.h"
#include <vector>

#define CONSOLEPANEL "Console##"
#define VECTOR_LENGH 300

class ConsolePanel : public Panel
{
public:
	ConsolePanel();
	~ConsolePanel();

	void Draw(int windowFlags) override;
	void SetLog(const char* logBuffer);

private:
	std::vector<const char*> log;
};