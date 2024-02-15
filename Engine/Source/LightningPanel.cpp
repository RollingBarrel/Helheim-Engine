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
		openGl->mDLightUniBuffer->UpdateData(&openGl->mDirAmb.mDirCol[3], sizeof(DirectionalAmbient::mDirCol[3]), offsetof(DirectionalAmbient, mDirCol[3]));
	}
	if (ImGui::DragFloat3("DirLDir", openGl->mDirAmb.mDirDir, 0.05f, -1.0f, 1.0f))
	{ 
		openGl->mDLightUniBuffer->UpdateData(openGl->mDirAmb.mDirDir, sizeof(DirectionalAmbient::mDirDir), offsetof(DirectionalAmbient, mDirDir));
	}
	if (ImGui::ColorPicker3("DirLCol", openGl->mDirAmb.mDirCol))
	{ 
		openGl->mDLightUniBuffer->UpdateData(openGl->mDirAmb.mDirCol, sizeof(DirectionalAmbient::mDirCol), offsetof(DirectionalAmbient, mDirCol));
	}
	if (ImGui::ColorPicker3("AmbientLCol", openGl->mDirAmb.mAmbientCol))
	{ 
		openGl->mDLightUniBuffer->UpdateData(openGl->mDirAmb.mAmbientCol, sizeof(DirectionalAmbient::mAmbientCol), offsetof(DirectionalAmbient, mAmbientCol));
	}
	ImGui::End();
	glUseProgram(0);
}
