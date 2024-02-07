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

	ImGui::PushItemWidth(400.0f);

	static ModuleTimer* clock = App->GetClock();

	static float fps;
	static long ms;
	static std::vector<float> fpsLog;
	static std::vector<unsigned long> msLog;
	
	//Executes evey 500 ms (no change if executed every time)
	if(clock->UpdateFpsLog())
	{
		fpsLog = clock->GetFpsLog();
		clock->FpsLogUpdated();

		ms = clock->GetRealDelta();
		fps = clock->GetFPS();
	}

	static int fps_limit = clock->GetFpsLimit();
	ImGui::SliderInt("FPS Limit", &fps_limit, 10, 60);
	clock->SetFpsLimit(fps_limit);

	long framedelay = 0;
	ImGui::Text("Last frame delayed for: %lld", framedelay);

	long lowfps = 0;
	float lowsec = 0;
	ImGui::Text("Lowest FPS: %lld on second %.1f", lowfps, lowsec);

	long slowframe = 0;
	float slowframeN = 0;
	ImGui::Text("Slowest frame: %lld MS on frame %.1f", slowframe, slowframeN);

	float realtime = 0;
	ImGui::Text("Real time since start: %.3f", realtime);

	float realtimedt = 0;
	ImGui::Text("Real time dt: %.3f", realtimedt);

	float gametime = 0;
	ImGui::Text("Real time since start: %.3f", gametime);

	float gametimedt = 0;
	ImGui::Text("Real time dt: %.3f", gametimedt);

	static float gameScale = 1;
	ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10);

	ImGui::PopItemWidth();

	ImGui::Text("Application average %lld ms/frame (%.1f FPS)", ms, fps);

	ImGui::Text("Total Time: %.2f", clock->GetTotalFrames() * 0.01666f);

	ImGui::Text("Frame Count: %u", clock->GetTotalFrames());

	ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));

	msLog = clock->GetMsLog();
	std::vector<float> msLogFloat(msLog.begin(), msLog.end());

	ImGui::PlotLines("MS", msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));
	ImGui::Text(" ");

	ImGui::End();
}