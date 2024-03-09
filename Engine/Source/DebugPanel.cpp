#include "DebugPanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleDebugDraw.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "Panel.h"
#include "Timer.h"
#include "TimerPanel.h"
#include "HierarchyPanel.h"
#include "ResourceMesh.h"

DebugPanel::DebugPanel() : Panel(DEBUGPANEL, false)
{
}

DebugPanel::~DebugPanel()
{
}

void DebugPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
        if (ImGui::TreeNode("Editor Scene##2"))
        {
            ImGui::Text("Render Mode");
            static const char* mRenderOptions[3] = { "Shaded", "Wireframe", "Shaded + Wireframe" };
            ImGui::Combo(" ", &mRenderMode, mRenderOptions, IM_ARRAYSIZE(mRenderOptions));
            if (ImGui::Checkbox("Draw Colliders", &mDrawColliders))
            {
                GameObject* root = App->GetScene()->GetRoot();
                SetShouldDrawForAll(root, mDrawColliders);
            }
            ImGui::Text("Total number of triangles on scene: %i", GetTotalTriangleCount(App->GetScene()->GetRoot()));
            ImGui::TreePop();
		}

        if (ImGui::TreeNode("Frames##2"))
        {
            if (ImGui::Checkbox("Show fps on editor", &mShowFpsOnEditor))
            {
                // TODO Add the code to show the FPS on the editor window (top right corner)
            }
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

void DebugPanel::SetShouldDrawForAll(GameObject* root, bool shouldDraw) {
    if (root != nullptr) {
        MeshRendererComponent* renderer = root->GetMeshRenderer();
        if (renderer != nullptr) {
            renderer->SetShouldDraw(shouldDraw);
        }

        for (int i = 0; i < root->GetChildren().size(); i++) {
            SetShouldDrawForAll(root->GetChildren()[i], shouldDraw);
        }
    }
}

int DebugPanel::GetTotalTriangleCount(GameObject* root) {
	int total = 0;
	if (root != nullptr) {
		MeshRendererComponent* renderer = root->GetMeshRenderer();
		if (renderer != nullptr) {
			total += renderer->GetResourceMesh()->GetNumberIndices() / 3;
		}

		for (int i = 0; i < root->GetChildren().size(); i++) {
			total += GetTotalTriangleCount(root->GetChildren()[i]);
		}
	}
	return total;
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