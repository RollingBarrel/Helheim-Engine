#include "DebugPanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "Timer.h"
#include "TimerPanel.h"

DebugPanel::DebugPanel() : Panel(DEBUGPANEL, false)
{
}

DebugPanel::~DebugPanel()
{
}

void DebugPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
        static float fps;
        static long long unitSecs;
        static long long averageUnitSecs;

        static std::vector<float> fpsLog;
        static std::vector<unsigned long long> msLog = App->GetCurrentClock()->GetMsLog();;

        //Timer precision (turns microseconds into milliseconds if the timer is in micros)
        static float precision = 1000.0f / App->GetCurrentClock()->GetTimerPrecision();
        const char* timeUnit = "micro secs";
        if (precision == 1) {
            timeUnit = "ms";
        }

        //Executes evey 500 ms (no change if executed every time)
        if (App->GetCurrentClock()->UpdateFpsLog())
        {
            fpsLog = App->GetCurrentClock()->GetFpsLog();
            App->GetCurrentClock()->FpsLogUpdated();

            averageUnitSecs = unitSecs;
            fps = App->GetCurrentClock()->GetFPS();
        }

        if (ImGui::TreeNode("Editor Scene##2"))
        {
            ImGui::Text("Render Mode");
            static const char* mRenderOptions[3] = { "Shaded", "Wireframe", "Shaded + Wireframe" };
            ImGui::Combo(" ", &mRenderMode, mRenderOptions, IM_ARRAYSIZE(mRenderOptions));
            bool drawColliders = false;
            if (ImGui::Checkbox("Draw Colliders", &drawColliders))
            {
                drawColliders = !drawColliders;
                // TODO Add the code to draw colliders
            }
            // TODO Add a function to calculate the number of triangles on the editor scene
            ImGui::Text("Number of triangles: %i", 0);
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Frames##2"))
        {
            bool showFPS = false;
            if (ImGui::Checkbox("Show fps on editor", &showFPS))
            {
                showFPS = !showFPS;
                // TODO Add the code to draw colliders
            }
            ImGui::Text("Frame Count: %u", App->GetCurrentClock()->GetTotalFrames());
            ImGui::Text("Lowest FPS: %.2f on second %.2f", App->GetCurrentClock()->GetLowestFPS(), App->GetCurrentClock()->GetLowestFpsTime() / App->GetCurrentClock()->GetTimerPrecision());
            ImGui::PlotHistogram("FPS", fpsLog.data(), fpsLog.size(), 0, NULL, 0.0f, 100.0f, ImVec2(0, 80));
            ImGui::Text("Slowest frame: %lld %s on frame %i", App->GetCurrentClock()->GetSlowestFrameTime(), timeUnit, App->GetCurrentClock()->GetSlowestFrame());
            if (App->GetCurrentClock() == App->GetGameClock())
            {
                ImGui::Text("Game delta time: %d %s", App->GetCurrentClock()->GetDelta(), timeUnit);
            }
            else
            {
				ImGui::Text("Game delta time: - ms");
			}
            ImGui::Text("Real delta time: %d %s", unitSecs, timeUnit);
            std::vector<float> msLogFloat(msLog.begin(), msLog.end());
            ImGui::PlotLines("ms", msLogFloat.data(), msLogFloat.size(), 0, NULL, FLT_MAX, FLT_MAX, ImVec2(400, 50));
            ImGui::Text("Application average %lld %s/frame (%.1f FPS)", averageUnitSecs, timeUnit, fps);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Timer##2"))
        {
            if (App->GetCurrentClock() == App->GetGameClock())
            {
                if (App->GetCurrentClock()->GetNewSpeed() != 0) {
                    static float gameScale;
                    gameScale = App->GetCurrentClock()->GetNewSpeed();
                    ImGui::SliderFloat("Game Clock Scale", &gameScale, 0.1f, 10, "%.1f");
                    App->GetCurrentClock()->SetTimeScale(gameScale);
                    if (ImGui::Button("-") && gameScale != 0.1f) {
                        App->GetCurrentClock()->SetTimeScale(gameScale - 0.1f);
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
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Quadtree##2"))
        {
            bool draw = App->GetScene()->GetShouldRenderQuadtree();
            if (ImGui::Checkbox("Draw quadtree", &draw))
                App->GetScene()->SetShouldRenderQuadtree(draw);

            ImGui::Separator();
            ImGui::SameLine();
            if (ImGui::Button("Reload quadtree"))
            {
                App->GetScene()->GetQuadtreeRoot()->UpdateTree();
            }
            ImGui::Separator();
            ImGui::Text("Quadtree nodes:");
            App->GetScene()->GetQuadtreeRoot()->RenderTreeImGui();
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Others##2"))
        {
            if (ImGui::Checkbox("Draw Mouse Picking RayCast", &mDrawRaycast)) {
                App->GetCamera()->DrawRayCast(mDrawRaycast);
            }
            ImGui::TreePop();
        }
	}
	ImGui::End();
}
/*/
    Settings
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

    bool grid = App->GetDebugDraw()->GetShouldRenderGrid();
    if (ImGui::Checkbox("Draw Grid", &grid)) {
        App->GetDebugDraw()->SetRenderGrid(grid);
    }
*/