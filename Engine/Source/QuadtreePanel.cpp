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
		if(ImGui::Button("Clean quadtree"))
		{
			App->GetScene()->GetQuadtreeRoot()->CleanUp();
		}
		
		ImGui::Text("My Quadtree:");
		App->GetScene()->GetQuadtreeRoot()->RenderTreeImGui();
		
		
	}
	ImGui::End();
}
