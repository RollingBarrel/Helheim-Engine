#pragma once
#include "Panel.h"
#define TIMERPANEL "Timer##"

class TimerPanel : public Panel
{
public:
	TimerPanel();
	~TimerPanel();
	void Draw(int windowFlags) override;
};

