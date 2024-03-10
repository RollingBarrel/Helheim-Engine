#include "SettingsPanel.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Quadtree.h"

#include "imgui.h"

SettingsPanel::SettingsPanel() : Panel(SETTINGSPANEL, false)
{
}

SettingsPanel::~SettingsPanel()
{
}

void SettingsPanel::Draw(int windowFlags) {
	if (ImGui::Begin(GetName(), &mOpen, windowFlags))
	{
		bool culling = App->GetScene()->GetApplyFrustumCulling();
		if (ImGui::Checkbox("Apply frustum culling", &culling))
			App->GetScene()->SetApplyFrustumCulling(culling);



	}
	ImGui::End();
}