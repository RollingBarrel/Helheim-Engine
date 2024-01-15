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
		ImGui::Checkbox("Draw quadtree", App->GetScene()->GetShouldRenderQuadtree());
		
		ImGui::Text("My Quadtree:");
		App->GetScene()->GetQuadtreeRoot()->RenderTreeImGui();
		
		
	}
	ImGui::End();
}
