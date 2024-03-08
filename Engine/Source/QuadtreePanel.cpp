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
		
	}
	ImGui::End();
}
