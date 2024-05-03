#include "LightningPanel.h"
#include "EngineApp.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"
#include <ImGuiFileDialog.h>


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, false) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleOpenGL* openGl = EngineApp->GetOpenGL();

	glUseProgram(EngineApp->GetOpenGL()->GetPBRProgramId());
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (ImGui::Button("Bake Ambient Light"))
	{
		App->GetOpenGL()->BakeIBL(mSkyboxFileName.c_str(), mIrradianceSize, mSpecEnvBRDFSize, mSpecPrefilteredSize);
	}

	ImGui::Text(mSkyboxFileName.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Select Skybox"))
	{
		IGFD::FileDialogConfig config;
		config.path = "./Assets/Textures";
		ImGuiFileDialog::Instance()->OpenDialog("SaveScene", "Choose File", ".hdr", config);
	}

	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
	if (ImGuiFileDialog::Instance()->Display("SaveScene"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			const std::string& filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			App->GetOpenGL()->BakeIBL(filePathName.c_str(), mIrradianceSize, mSpecEnvBRDFSize, mSpecPrefilteredSize);
			mSkyboxFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			mSkyboxFilePath = filePathName;
		}

		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::InputInt("IrradianceSize", &mIrradianceSize);
	ImGui::InputInt("SpecularPrefilteredSize", &mSpecPrefilteredSize);
	ImGui::InputInt("BRDFEnvSize", &mSpecEnvBRDFSize);

	

	
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
