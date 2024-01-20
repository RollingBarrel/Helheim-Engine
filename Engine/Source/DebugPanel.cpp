#include "DebugPanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"

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
		




	}
	ImGui::End();
}