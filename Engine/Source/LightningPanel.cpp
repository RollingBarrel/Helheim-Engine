#include "LightningPanel.h"
#include "Application.h"
#include "ModuleRenderTest.h"
#include "imgui.h"


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleRenderTest* renderTest = App->GetTest();

	ImGui::Begin(GetName(), &mOpen, windowFlags);
	
	if (ImGui::DragFloat("KD", &renderTest->kD, 0.05f, 0.0f, 1.0f))
	{
		renderTest->mModified = true;
	}
	if (ImGui::DragFloat("Brightness", &renderTest->brightness, 0.05f, 0.0f))
	{
		renderTest->mModified = true;
	}
	if (ImGui::DragFloat3("LightDir", renderTest->lightDir, 0.05f, -1.0f, 1.0f))
	{ 
		renderTest->mModified = true;
	}
	if (ImGui::ColorPicker3("LightCol", renderTest->lightCol))
	{ 
		renderTest->mModified = true;
	}
	if (ImGui::ColorPicker3("AmbientCol", renderTest->ambientCol))
	{ 
		renderTest->mModified = true;
	}

	ImGui::End();
}
