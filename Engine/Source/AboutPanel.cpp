#include "AboutPanel.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"

#include "imgui.h"

AboutPanel::AboutPanel() : Panel(ABOUTPANEL, true)
{
}

AboutPanel::~AboutPanel()
{
}

void AboutPanel::Draw(int windowFlags) 
{
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), ImGui::GetWindowSize(), ImVec2(0,1), ImVec2(1,0));
		//ImGui::Text("Welcum");
	}
	ImGui::End();
}
