#include "QuadtreePanel.h"
#include "imgui.h"
#include "Quadtree.h"
#include "Application.h"
#include "ModuleScene.h"


QuadtreePanel::QuadtreePanel() : Panel(QUADTREEPANEL, false)
{
}

QuadtreePanel::~QuadtreePanel()
{
}

void QuadtreePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		bool culling = App->GetScene()->GetApplyFrustumCulling();
		if (ImGui::Checkbox("Apply frustum culling", &culling))
			App->GetScene()->SetApplyFrustumCulling(culling);

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
		ImGui::Text("My Quadtree:");
		App->GetScene()->GetQuadtreeRoot()->RenderTreeImGui();
		
		
	}
	ImGui::End();
}
