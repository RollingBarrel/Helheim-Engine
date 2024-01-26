#include "LightningPanel.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleOpenGL* mOpenGl = App->GetOpenGL();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	
	if (ImGui::DragFloat("LightIntensity", &mOpenGl->mLightIntensity, 0.05f, 0.0f))
	{
		glUniform1f(5, mOpenGl->mLightIntensity);
	}
	if (ImGui::DragFloat3("LightDir", mOpenGl->mLightDir, 0.05f, -1.0f, 1.0f))
	{ 
		glUniform3fv(1, 1, mOpenGl->mLightDir);
	}
	if (ImGui::ColorPicker3("LightCol", mOpenGl->mLightCol))
	{ 
		glUniform3fv(3, 1, mOpenGl->mLightCol);
	}
	if (ImGui::ColorPicker3("AmbientCol", mOpenGl->mAmbientCol))
	{ 
		glUniform3fv(4, 1, mOpenGl->mAmbientCol);
	}	
	ImGui::End();
	glUseProgram(0);
}
