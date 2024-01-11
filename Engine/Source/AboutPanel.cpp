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
		//LOG("%d", App->GetOpenGL()->GetFramebufferTexture());


		ImGui::Image((void*)(intptr_t)App->GetOpenGL()->GetFramebufferTexture(), ImVec2(App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight()));
		//mGui::Text("Welcum");
	}
	ImGui::End();
}
