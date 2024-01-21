#include "LightningPanel.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "imgui.h"


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleProgram* program = App->GetProgram();

	ImGui::Begin(GetName(), &mOpen, windowFlags);
	

	if (ImGui::DragFloat("LightIntensity", &program->mLightIntensity, 0.05f, 0.0f))
	{
		program->mModified = true;
	}
	if (ImGui::DragFloat3("LightDir", program->mLightDir, 0.05f, -1.0f, 1.0f))
	{ 
		program->mModified = true;
	}
	if (ImGui::ColorPicker3("LightCol", program->mLightCol))
	{ 
		program->mModified = true;
	}
	if (ImGui::ColorPicker3("AmbientCol", program->mAmbientCol))
	{ 
		program->mModified = true;
	}	
	ImGui::End();
}
