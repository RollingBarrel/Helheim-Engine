#include "LightningPanel.h"
#include "EngineApp.h"
#include "ModuleOpenGL.h"
#include "imgui.h"
#include "glew.h"
#include <ImGuiFileDialog.h>


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleOpenGL* openGl = EngineApp->GetOpenGL();

	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (ImGui::Button("Bake Ambient Light"))
	{
		openGl->BakeIBL(mSkyboxFileName.c_str(), mIrradianceSize, mSpecEnvBRDFSize, mSpecPrefilteredSize);
	}

	ImGui::Text(mSkyboxFileName.c_str());
	ImGui::SameLine();
	if (ImGui::Button("Select Skybox"))
	{
		IGFD::FileDialogConfig config;
		config.path = "./Assets/Textures";
		ImGuiFileDialog::Instance()->OpenDialog("Skybox", "Choose Skybox File", ".hdr", config);
	}

	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
	if (ImGuiFileDialog::Instance()->Display("Skybox"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			const std::string& filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			openGl->BakeIBL(filePathName.c_str(), mIrradianceSize, mSpecEnvBRDFSize, mSpecPrefilteredSize);
			mSkyboxFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			mSkyboxFilePath = filePathName;
		}

		ImGuiFileDialog::Instance()->Close();
	}
	ImGui::InputInt("IrradianceSize", &mIrradianceSize);
	ImGui::InputInt("SpecularPrefilteredSize", &mSpecPrefilteredSize);
	ImGui::InputInt("BRDFEnvSize", &mSpecEnvBRDFSize);

	

	
	if (ImGui::DragFloat("DirLIntensity", &openGl->mDirLight.mCol[3], 0.05f, 0.0f, 100.0f))
	{
		openGl->mDLightUniBuffer->UpdateData(&openGl->mDirLight.mCol[3], sizeof(DirectionalLight::mCol[3]), offsetof(DirectionalLight, mCol[3]));
	}
	if (ImGui::DragFloat3("DirLDir", openGl->mDirLight.mDir, 0.05f, -1.0f, 1.0f))
	{ 
		openGl->mDLightUniBuffer->UpdateData(openGl->mDirLight.mDir, sizeof(DirectionalLight::mDir), offsetof(DirectionalLight, mDir));
	}
	if (ImGui::ColorPicker3("DirLCol", openGl->mDirLight.mCol))
	{ 
		openGl->mDLightUniBuffer->UpdateData(openGl->mDirLight.mCol, sizeof(DirectionalLight::mCol), offsetof(DirectionalLight, mCol));
	}
	ImGui::Text("Ambient Occlusion");
	if (ImGui::Checkbox("AO", &openGl->mAoActive))
	{
		glUseProgram(openGl->GetPbrLightingPassProgramId());
		glUniform1i(glGetUniformLocation(openGl->GetPbrLightingPassProgramId(), "activeAO"),openGl->mAoActive);
		glUseProgram(0);

	}
	float aoRange = openGl->GetAoRange();
	float aoBias = openGl->GetAoBias();
	if (ImGui::DragFloat("Range", &aoRange, 0.05f, 0.0f, 100.0f))
	{
		openGl->SetAoRange(aoRange);
		glUseProgram(openGl->GetSSAOProgramId());
		glUniform1f(1, aoRange);
		glUseProgram(0);
	}
	if (ImGui::DragFloat("Bias", &aoBias, 0.05f, 0.0f, 100.0f))
	{
		openGl->SetAoBias(aoBias);
		glUseProgram(openGl->GetSSAOProgramId());
		glUniform1f(2, aoBias);
		glUseProgram(0);
	}
	ImGui::End();
}
