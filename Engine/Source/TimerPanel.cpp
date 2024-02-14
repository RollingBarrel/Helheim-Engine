#include "TimerPanel.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"

#include "Timer.h"
#include "PreciseTimer.h"


TimerPanel::TimerPanel() : Panel(TIMERPANEL, true) 
{
}

TimerPanel::~TimerPanel()
{
}


void TimerPanel::Draw(int windowFlags)
{
	static Timer* clock = App->GetEngineClock();

	ImGui::Begin(GetName(), &mOpen, windowFlags);

	ImGui::PushItemWidth(300.0f);

	const char* timers[] = { "Engine", "Game"};
	static int item_current = 0;
	ImGui::Combo("Current Timer", &item_current, timers, 2);
	ImGui::Text("   ");

	if (item_current == 0) 
	{
		clock = App->GetEngineClock();
	}
	else 
	{
		clock = App->GetGameClock();
	}

	static float fps;
	static long ms;
	static std::vector<float> fpsLog;
	static std::vector<unsigned long> msLog;
	
	//Executes evey 500 ms (no change if executed every time)
	if(clock->UpdateFpsLog())
	{
		fpsLog = clock->GetFpsLog();
		clock->FpsLogUpdated();

		ms = clock->GetDelta();
		fps = clock->GetFPS();
	}

	ImGui::SeparatorText("FPS");

	int fps_limit = clock->GetFpsLimit();
	ImGui::SliderInt("FPS Limit", &fps_limit, 10, 60);
	clock->SetFpsLimit(fps_limit);

	ImGui::Text("Lowest FPS: %u on second %.3f", clock->GetLowestFPS(), clock->GetLowestFpsTime()/1000.f);

	ImGui::SeparatorText("Frames");

	ImGui::Text("Last frame delayed for %d ms", clock->GetFrameDelay());

	ImGui::Text("Slowest frame: %d MS on frame %i", clock->GetSlowestFrameTime(), clock->GetSlowestFrame());

	ImGui::Text("Delta time: %d ms", clock->GetDelta());

	ImGui::Text("Frame Count: %u", clock->GetTotalFrames());

	ImGui::SeparatorText("Time");

	if (item_current == 1)
	{
		ImGui::Text("Real time since start: %.2f seconds", clock->GetRealTime() / 1000.0f);
	}

	ImGui::Text("Total Time: %.2f seconds", clock->GetTotalTime() / 1000.0f);

	if (item_current == 1 && clock->GetNewSpeed() != 0) {
		static float gameScale;
		gameScale = clock->GetNewSpeed();
		ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10, "%.1f");
		clock->SetTimeScale(gameScale);
	}

	ImGui::PopItemWidth();

	//Plots
	ImGui::SeparatorText("Graphs");

	ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	msLog = clock->GetMsLog();
	std::vector<float> msLogFloat(msLog.begin(), msLog.end());

	ImGui::PlotLines("MS", msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	ImGui::Text("Application average %lld ms/frame (%.1f FPS)", ms, fps);

	ImGui::End();
}