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
	
	if (ImGui::DragFloat("DirLIntensity", &openGl->mDirAmb.mDirCol[3], 0.05f, 0.0f, 5.0f))
	{
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(DirectionalAmbient, mDirCol[3]), sizeof(DirectionalAmbient::mDirCol[3]), &openGl->mDirAmb.mDirCol[3]);
	}
	if (ImGui::DragFloat3("DirLDir", openGl->mDirAmb.mDirDir, 0.05f, -1.0f, 1.0f))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(DirectionalAmbient, mDirDir), sizeof(DirectionalAmbient::mDirDir), App->GetOpenGL()->mDirAmb.mDirDir);
	}
	if (ImGui::ColorPicker3("DirLCol", openGl->mDirAmb.mDirCol))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(DirectionalAmbient, mDirCol), sizeof(DirectionalAmbient::mDirCol), openGl->mDirAmb.mDirCol);
	}
	if (ImGui::ColorPicker3("AmbientLCol", openGl->mDirAmb.mAmbientCol))
	{ 
		glBindBuffer(GL_UNIFORM_BUFFER, openGl->lightUnis);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(DirectionalAmbient, mAmbientCol), sizeof(DirectionalAmbient::mAmbientCol), openGl->mDirAmb.mAmbientCol);
	}
	ImGui::End();
	glUseProgram(0);
}
