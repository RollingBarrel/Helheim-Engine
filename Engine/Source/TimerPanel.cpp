#include "TimerPanel.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"

#include "ModuleTimer.h"
#include "PreciseTimer.h"


TimerPanel::TimerPanel() : Panel(TIMERPANEL, true) 
{
}

TimerPanel::~TimerPanel()
{
}


void TimerPanel::Draw(int windowFlags)
{
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	static ModuleTimer* clock = App->GetClock();

	static float fps;
	static std::vector<float> fpsLog;
	
	if(clock->UpdateFpsLog())
	{
		fpsLog = clock->GetFpsLog();
		clock->FpsLogUpdated();
	}

	ImGui::PlotHistogram("FPS Histogram", fpsLog.data(), fpsLog.size(), 0, NULL, 0, clock->GetFpsLimit()+10, ImVec2(0, 150));

	fps = clock->GetFPS();
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / fps, fps);



	ImGui::Text(" ");

	ImGui::End();
}