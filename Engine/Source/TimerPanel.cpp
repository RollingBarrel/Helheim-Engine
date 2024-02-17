#include "TimerPanel.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"

#include "Timer.h"
#include "PreciseTimer.h"

#define STANDARD_FPS_LIMIT 60


TimerPanel::TimerPanel() : Panel(TIMERPANEL, false) 
{
}

TimerPanel::~TimerPanel()
{
}


void TimerPanel::Draw(int windowFlags)
{
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	ImGui::PushItemWidth(300.0f);

	ImGui::SeparatorText("Current Timer");
	static const char* timerName;
	if (App->GetCurrentClock() == App->GetEngineClock()) {
		timerName = "Engine";
	}
	else {
		timerName = "Game";
	}
	ImGui::Text(timerName);

	static float fps;
	static long ms;
	static long averageMs;
	static std::vector<float> fpsLog;
	static std::vector<unsigned long> msLog;
	static bool fpsLimitEnabled = true;
	static bool vsyncEnabled = true;
	
	ms = App->GetCurrentClock()->GetDelta() / (float)App->GetCurrentClock()->GetSpeed();

	//Executes evey 500 ms (no change if executed every time)
	if(App->GetCurrentClock()->UpdateFpsLog())
	{
		fpsLog = App->GetCurrentClock()->GetFpsLog();
		App->GetCurrentClock()->FpsLogUpdated();

		averageMs = ms;
		fps = App->GetCurrentClock()->GetFPS();
	}

	ImGui::SeparatorText("Vsync");
	ImGui::Checkbox("Vsync enabled", &vsyncEnabled);
	if (vsyncEnabled != App->GetCurrentClock()->GetVsyncStatus()) {
		App->GetCurrentClock()->SetVsyncStatus(vsyncEnabled);
	}

	ImGui::SeparatorText("FPS");
	if (!vsyncEnabled) {
		ImGui::Checkbox("Enable FPS Limit", &fpsLimitEnabled);
		int fps_limit = 0;
		if (fpsLimitEnabled) {
			fps_limit = App->GetCurrentClock()->GetFpsLimit();
			ImGui::SliderInt("FPS Limit", &fps_limit, 10, 240);
			
		}
		App->GetCurrentClock()->SetFpsLimit(fps_limit);
		
	}

	ImGui::Text("Lowest FPS: %.2f on second %.2f", App->GetCurrentClock()->GetLowestFPS(), App->GetCurrentClock()->GetLowestFpsTime()/1000.f);

	ImGui::SeparatorText("Frames");

	ImGui::Text("Last frame delayed for %d ms", App->GetCurrentClock()->GetFrameDelay());
	ImGui::Text("Actual execution time of the last frame: %d ms", ms - App->GetCurrentClock()->GetFrameDelay());

	ImGui::Text("Slowest frame: %d MS on frame %i", App->GetCurrentClock()->GetSlowestFrameTime(), App->GetCurrentClock()->GetSlowestFrame());

	if (App->GetCurrentClock() == App->GetGameClock())
	{
		ImGui::Text("Game delta time: %d ms", App->GetCurrentClock()->GetDelta());
	}

	ImGui::Text("Real delta time: %d ms", ms);

	ImGui::Text("Frame Count: %u", App->GetCurrentClock()->GetTotalFrames());

	ImGui::SeparatorText("Time");

	if (App->GetCurrentClock() == App->GetGameClock())
	{
		if (App->GetCurrentClock()->GetNewSpeed() != 0) {
			static float gameScale;
			gameScale = App->GetCurrentClock()->GetNewSpeed();
			ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10, "%.1f");
			App->GetCurrentClock()->SetTimeScale(gameScale);
			if (ImGui::Button("-")) {
				App->GetCurrentClock()->SetTimeScale(gameScale-0.1f);
			}ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				App->GetCurrentClock()->SetTimeScale(1.f);
			}ImGui::SameLine();
			if (ImGui::Button("+")) {
				App->GetCurrentClock()->SetTimeScale(gameScale + 0.1f);
			}
			ImGui::Text(" ");
		}

		ImGui::Text("Real time since start: %.2f", App->GetCurrentClock()->GetRealTime() / 1000.0f);
	}

	ImGui::Text("Total Time: %.2f", App->GetCurrentClock()->GetTotalTime() / 1000.0f);

	ImGui::PopItemWidth();

	//Plots
	ImGui::SeparatorText("Graphs");

	ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	msLog = App->GetCurrentClock()->GetMsLog();
	std::vector<float> msLogFloat(msLog.begin(), msLog.end());

	ImGui::PlotLines("MS", msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	ImGui::Text("Application average %lld ms/frame (%.1f FPS)", averageMs, fps);

	ImGui::End();
}