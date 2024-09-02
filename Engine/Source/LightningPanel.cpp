#include "LightningPanel.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
#include "ModuleOpenGL.h"
#include "ModuleFileSystem.h"
#include "Archive.h"
#include "imgui.h"
#include "glew.h"
#include <ImGuiFileDialog.h>
#include "ImporterIBL.h"


LightningPanel::LightningPanel() : Panel(LIGHTNINGPANEL, true) {}

void LightningPanel::Draw(int windowFlags)
{
	ModuleOpenGL* openGl = EngineApp->GetOpenGL();

	ImGui::Begin(GetName(), &mOpen, windowFlags);

	ImGui::Text("Skybox and IBL");
	//ImGui::Text(mSkyboxFileName.c_str());
	if (ImGui::Button("Select Skybox"))
	{
		IGFD::FileDialogConfig config;
		config.path = "./Assets/IBL";
		ImGuiFileDialog::Instance()->OpenDialog("Skybox", "Choose Skybox File", ".hdr", config);
	}
	
	ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
	if (ImGuiFileDialog::Instance()->Display("Skybox"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ASSETS_IBL_PATH;
			filePathName += ImGuiFileDialog::Instance()->GetCurrentFileName();
			Resource* res = EngineApp->GetEngineResource()->RequestResource(filePathName.c_str());
			if (res)
			{
				App->GetOpenGL()->SetSkybox(res->GetUID());
				//mSkyboxFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
				App->GetResource()->ReleaseResource(res->GetUID());
			}
		}
	
		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine();
	if (ImGui::Button("RemoveSkybox"))
	{
		openGl->SetSkybox(0);
		//mSkyboxFileName = "";
	}
	//ImGui::InputInt("IrradianceSize", &mIrradianceSize);
	//ImGui::InputInt("SpecularPrefilteredSize", &mSpecPrefilteredSize);
	//ImGui::InputInt("BRDFEnvSize", &mSpecEnvBRDFSize);

	

	ImGui::Separator();
	ImGui::Text("Directional Light");
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

	ImGui::Separator();
	ImGui::Text("Bloom");
	float bloomIntensity = openGl->GetBloomIntensity();
	if (ImGui::DragFloat("Intensity", &bloomIntensity, 0.01f, 0.0f, 1.0f))
	{
		openGl->SetBloomIntensity(bloomIntensity);
	}


	ImGui::Separator();
	ImGui::Text("Fog");
	float tmp[3] = { 1.0f, 1.0f, 1.0f };
	openGl->GetFogColor(tmp);
	if (ImGui::ColorPicker3("Fog Color", tmp))
	{
		openGl->SetFogColor(tmp);
	}
	tmp[0] = openGl->GetMaxFog();
	if (ImGui::DragFloat("Max Fog", tmp, 0.01f, 0.0001f, 1.0f))
	{
		openGl->SetMaxFog(tmp[0]);
	}
	tmp[0] = openGl->GetFogDensity();
	if (ImGui::DragFloat("Density", tmp, 0.0001f, 0.0001f, 0.04f))
	{
		openGl->SetFogDensity(tmp[0]);
	}
	tmp[0] = openGl->GetFogHeightFallof();
	if (ImGui::DragFloat("Height Fallof", tmp, 0.005f, 0.0001f, 1.0f))
	{
		openGl->SetFogHeightFallof(tmp[0]);
	}

	ImGui::Separator();
	ImGui::Text("Volumetric");
	if (ImGui::DragFloat("baseExtCoeff", &openGl->mBaseExtCoeff, 0.05f, 0.0f, 1.0f))
	{
		glUseProgram(openGl->mVolLightProgramId);
		glUniform1f(0, openGl->mBaseExtCoeff);
		glUseProgram(0);
	}
	if (ImGui::DragFloat("Noise Amount", &openGl->mNoiseAmount, 0.05f, 0.0f, 1.0f))
	{
		glUseProgram(openGl->mVolLightProgramId);
		glUniform1f(0, openGl->mNoiseAmount);
		glUseProgram(0);
	}
	if (ImGui::DragFloat("VolIntensity", &openGl->mVolIntensity, 0.05f, 0.0f, 1.0f))
	{
		glUseProgram(openGl->mVolLightProgramId);
		glUniform1f(0, openGl->mVolIntensity);
		glUseProgram(0);
	}
	if (ImGui::DragFloat("Anisotropy (Mie scattering)", &openGl->mVolAnisotropy, 0.05f, 0.0f, 1.0f))
	{
		glUseProgram(openGl->mVolLightProgramId);
		glUniform1f(4, openGl->mVolAnisotropy);
		glUseProgram(0);
	}
	if (ImGui::DragFloat("StepSize", &openGl->mVolStepSize, 0.01f))
	{
		glUseProgram(openGl->mVolLightProgramId);
		glUniform1f(8, openGl->mVolStepSize);
		glUseProgram(0);
	}

	ImGui::Separator();
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
