#include "ScenePanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"

#include "imgui.h"

ScenePanel::ScenePanel() : Panel(SCENEPANEL, true)
{
}

ScenePanel::~ScenePanel()
{
}

void ScenePanel::Draw(int windowFlags)
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImVec2 size = ImGui::GetContentRegionAvail();
		if (size.x != prevSizeX || size.y != prevSizeY)
		{
			App->GetOpenGL()->SceneFramebufferResized(size.x, size.y);
			prevSizeX = size.x;
			prevSizeY = size.y;
		}
		ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), size, ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();
}
