#include "TimerPanel.h"
#include "EngineApp.h"
#include "ModuleOpenGL.h"
#include "ModuleEditor.h"
#include "SettingsPanel.h"

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
	static float precision = 1000.0f / EngineApp->GetCurrentClock()->GetTimerPrecision();

	const char* timeUnit = "micro secs";
	if (precision == 1) {
		timeUnit = "ms";
	}

	ImGui::SeparatorText("Current Timer");

	static const char* timerName;
	if (EngineApp->GetCurrentClock() == EngineApp->GetEngineClock()) {
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
	
	unitSecs = EngineApp->GetCurrentClock()->GetDelta() / (float)EngineApp->GetCurrentClock()->GetSpeed();

	//Executes evey 500 ms (no change if executed every time)
	if(EngineApp->GetCurrentClock()->UpdateFpsLog())
	{
		fpsLog = EngineApp->GetCurrentClock()->GetFpsLog();
		EngineApp->GetCurrentClock()->FpsLogUpdated();

		averageUnitSecs = unitSecs;
		fps = EngineApp->GetCurrentClock()->GetFPS();
	}

	

	ImGui::SeparatorText("FPS");

	ImGui::Text("Lowest FPS: %.2f on second %.2f", EngineApp->GetCurrentClock()->GetLowestFPS(), EngineApp->GetCurrentClock()->GetLowestFpsTime() / EngineApp->GetCurrentClock()->GetTimerPrecision());

	ImGui::SeparatorText("Frames");

	if (!EngineApp->GetCurrentClock()->GetVsyncStatus()) //Only works without vsync
	{
		ImGui::Text("Last frame delayed for %d %s", EngineApp->GetCurrentClock()->GetFrameDelay(), timeUnit);
		ImGui::Text("Actual execution time of the last frame: %d %s", unitSecs - EngineApp->GetCurrentClock()->GetFrameDelay(), timeUnit);
	}


	ImGui::Text("Slowest frame: %lld %s on frame %i", EngineApp->GetCurrentClock()->GetSlowestFrameTime(), timeUnit, EngineApp->GetCurrentClock()->GetSlowestFrame());

	if (EngineApp->GetCurrentClock() == EngineApp->GetGameClock())
	{
		ImGui::Text("Game delta time: %d %s", EngineApp->GetCurrentClock()->GetDelta(), timeUnit);
	}

	ImGui::Text("Real delta time: %d %s", unitSecs, timeUnit);

	ImGui::Text("Frame Count: %u", EngineApp->GetCurrentClock()->GetTotalFrames());

	ImGui::SeparatorText("Time");

	if (EngineApp->GetCurrentClock() == EngineApp->GetGameClock())
	{
		if (EngineApp->GetCurrentClock()->GetNewSpeed() != 0) {
			static float gameScale;
			gameScale = EngineApp->GetCurrentClock()->GetNewSpeed();
			ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10, "%.1f");
			EngineApp->GetCurrentClock()->SetTimeScale(gameScale);
			if (ImGui::Button("-") && gameScale != 0.1f) {
				EngineApp->GetCurrentClock()->SetTimeScale(gameScale-0.1f);
			}ImGui::SameLine();
			if (ImGui::Button("Reset")) {
				EngineApp->GetCurrentClock()->SetTimeScale(1.f);
			}ImGui::SameLine();
			if (ImGui::Button("+")) {
				EngineApp->GetCurrentClock()->SetTimeScale(gameScale + 0.1f);
			}
			ImGui::Text(" ");
		}

		ImGui::Text("Real time since start: %.2f seconds", EngineApp->GetCurrentClock()->GetRealTime() * precision / 1000.0f);
	}

	ImGui::Text("Total Time: %.2f seconds", EngineApp->GetCurrentClock()->GetTotalTime() * precision / 1000.0f);

	ImGui::PopItemWidth();

	//Plots
	ImGui::SeparatorText("Graphs");

	ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	msLog = EngineApp->GetCurrentClock()->GetMsLog();
	std::vector<float> msLogFloat(msLog.begin(), msLog.end());

	ImGui::PlotLines(timeUnit, msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	ImGui::Text("EngineApplication average %lld %s/frame (%.1f FPS)", averageUnitSecs, timeUnit, fps);

	ImGui::End();
}