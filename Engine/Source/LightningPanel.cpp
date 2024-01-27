#include "LightningPanel.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleOpenGL* openGl = App->GetOpenGL();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());
	ImGui::Begin(GetName(), &mOpen, windowFlags);
	
	if (ImGui::DragFloat("LightIntensity", &openGl->mDirCol[3], 0.05f, 0.0f, 5.0f))
	{
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(openGl->mDirDir) + sizeof(float) * 3, sizeof(float), &openGl->mDirCol[3]);
	}
	if (ImGui::DragFloat3("LightDir", openGl->mDirDir, 0.05f, -1.0f, 1.0f))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, App->GetOpenGL()->mDirDir);
	}
	if (ImGui::ColorPicker3("LightCol", openGl->mDirCol))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, openGl->mDirCol);
	}
	if (ImGui::ColorPicker3("AmbientCol", openGl->mAmbientCol))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float) * 3, openGl->mAmbientCol);
	}	
	ImGui::End();
	glUseProgram(0);
}
