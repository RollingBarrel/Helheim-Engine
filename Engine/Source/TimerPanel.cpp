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

	//Timer precision (turns microseconds into milliseconds if the timer is in micros)
	static float precision = 1000.0f / App->GetCurrentClock()->GetTimerPrecision();

	const char* timeUnit = "micro secs";
	if (precision == 1) {
		timeUnit = "ms";
	}

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
	static long long unitSecs;
	static long long averageUnitSecs;

	static std::vector<float> fpsLog;
	static std::vector<unsigned long long> msLog;

	static bool fpsLimitEnabled = true;
	static bool vsyncEnabled = true;
	
	unitSecs = App->GetCurrentClock()->GetDelta() / (float)App->GetCurrentClock()->GetSpeed();

	//Executes evey 500 ms (no change if executed every time)
	if(App->GetCurrentClock()->UpdateFpsLog())
	{
		fpsLog = App->GetCurrentClock()->GetFpsLog();
		App->GetCurrentClock()->FpsLogUpdated();

		averageUnitSecs = unitSecs;
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

	ImGui::Text("Lowest FPS: %.2f on second %.2f", App->GetCurrentClock()->GetLowestFPS(), App->GetCurrentClock()->GetLowestFpsTime() / App->GetCurrentClock()->GetTimerPrecision());

	ImGui::SeparatorText("Frames");

	if (!vsyncEnabled) //Only works without vsync
	{
		ImGui::Text("Last frame delayed for %d %s", App->GetCurrentClock()->GetFrameDelay(), timeUnit);
		ImGui::Text("Actual execution time of the last frame: %d %s", unitSecs - App->GetCurrentClock()->GetFrameDelay(), timeUnit);
	}

	ImGui::Text("Slowest frame: %lld %s on frame %i", App->GetCurrentClock()->GetSlowestFrameTime(), timeUnit, App->GetCurrentClock()->GetSlowestFrame());

	if (App->GetCurrentClock() == App->GetGameClock())
	{
		ImGui::Text("Game delta time: %d %s", App->GetCurrentClock()->GetDelta(), timeUnit);
	}

	ImGui::Text("Real delta time: %d %s", unitSecs, timeUnit);

	ImGui::Text("Frame Count: %u", App->GetCurrentClock()->GetTotalFrames());

	ImGui::SeparatorText("Time");

	if (App->GetCurrentClock() == App->GetGameClock())
	{
		if (App->GetCurrentClock()->GetNewSpeed() != 0) {
			static float gameScale;
			gameScale = App->GetCurrentClock()->GetNewSpeed();
			ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10, "%.1f");
			App->GetCurrentClock()->SetTimeScale(gameScale);
			if (ImGui::Button("-") && gameScale != 0.1f) {
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

		ImGui::Text("Real time since start: %.2f seconds", App->GetCurrentClock()->GetRealTime() * precision / 1000.0f);
	}

	ImGui::Text("Total Time: %.2f seconds", App->GetCurrentClock()->GetTotalTime() * precision / 1000.0f);

	ImGui::PopItemWidth();

	//Plots
	ImGui::SeparatorText("Graphs");

	ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	msLog = App->GetCurrentClock()->GetMsLog();
	std::vector<float> msLogFloat(msLog.begin(), msLog.end());

	ImGui::PlotLines(timeUnit, msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	ImGui::Text("Application average %lld %s/frame (%.1f FPS)", averageUnitSecs, timeUnit, fps);

	ImGui::End();
}