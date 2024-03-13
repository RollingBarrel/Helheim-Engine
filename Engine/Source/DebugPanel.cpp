#include "DebugPanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleOpenGL.h"

DebugPanel::DebugPanel() : Panel(DEBUGPANEL, false)
{
}

DebugPanel::~DebugPanel()
{
}

void DebugPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		//ImGui::Checkbox("Draw Frustrum", App->GetCamera()->GetShouldRenderFrustum());
		bool grid = App->GetDebugDraw()->GetShouldRenderGrid();

		if (ImGui::Checkbox("Draw Grid", &grid)) {
			App->GetDebugDraw()->SetRenderGrid(grid);
		}
		if (ImGui::Checkbox("Draw Mouse Picking RayCast", &mDrawRaycast)) {
			App->GetCamera()->DrawRayCast(mDrawRaycast);
		}

		ImGui::Text("Render Mode");
		static const char* mRenderOptions[3] = { "Shaded", "Wireframe", "Shaded + Wireframe" };
		ImGui::Combo(" ", (int*)&mRenderMode, mRenderOptions, IM_ARRAYSIZE(mRenderOptions));
	}
	ImGui::End();
}