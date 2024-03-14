#include "DebugPanel.h"
#include "imgui.h"
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
		if (ImGui::Combo(" ", (int*)&mRenderMode, mRenderOptions, IM_ARRAYSIZE(mRenderOptions)))
		{
			switch(mRenderMode)
			{
			case RenderMode::Shaded:
				App->GetOpenGL()->SetWireframe(false);
				break;
			case RenderMode::Wireframe:
				App->GetOpenGL()->SetWireframe(true);
				break;
			case RenderMode::ShadedWireframe:
				//TODO Shaded + Wireframe rendering
				break;
			default:
				break;
			}
		}
	}
	ImGui::End();
}