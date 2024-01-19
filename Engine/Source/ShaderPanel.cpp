#include "ShaderPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"
#include "Mesh.h"
#include "Math/float4.h"
#include <MathFunc.h>
#include <string.h>


ShaderPanel::ShaderPanel() : Panel(SHADERPANEL, true)
{
}

ShaderPanel::~ShaderPanel()
{
}

void ShaderPanel::Draw(int windowFlags)
{
	GameObject* focusedObject = App->GetScene()->GetSelectedGameObject();
	char nameArray[100];
	strcpy_s(nameArray, focusedObject->mName.c_str());
	//ImGui::PushID(focusedObject->mID);
	//ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (!focusedObject->IsRoot())
	{
		if (focusedObject->getMeshRenderer() != nullptr)
		{
			static float lightDx = focusedObject->getMeshRenderer()->lightDir[0];
			static float lightDy = focusedObject->getMeshRenderer()->lightDir[1];
			static float lightDz = focusedObject->getMeshRenderer()->lightDir[2];

			ImGui::Text("Light Direction");
			if (ImGui::DragFloat("X light D", &lightDx, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightDir[0] = lightDx;
			}
			if (ImGui::DragFloat("Y light D", &lightDy, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightDir[1] = lightDy;
			}
			if (ImGui::DragFloat("Z light D", &lightDz, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightDir[2] = lightDz;
			}

			static float lightCx = focusedObject->getMeshRenderer()->lightColor[0];
			static float lightCy = focusedObject->getMeshRenderer()->lightColor[1];
			static float lightCz = focusedObject->getMeshRenderer()->lightColor[2];

			ImGui::Text("Light Color");
			if (ImGui::DragFloat("X light C", &lightCx, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightColor[0] = lightCx;
			}
			if (ImGui::DragFloat("Y light C", &lightCy, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightColor[1] = lightCy;
			}
			if (ImGui::DragFloat("Z light C", &lightCz, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightColor[2] = lightCz;
			}

			static float lightI = focusedObject->getMeshRenderer()->lightIntensity;

			ImGui::Text("Light Intensity");
			if (ImGui::DragFloat("intensity", &lightI, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->lightIntensity = lightI;
			}

			static float ambientCx = focusedObject->getMeshRenderer()->ambientColor[0];
			static float ambientCy = focusedObject->getMeshRenderer()->ambientColor[1];
			static float ambientCz = focusedObject->getMeshRenderer()->ambientColor[2];

			ImGui::Text("Ambient Color");
			if (ImGui::DragFloat("X ambient", &ambientCx, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->ambientColor[0] = ambientCx;
			}
			if (ImGui::DragFloat("Y ambient", &ambientCy, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->ambientColor[1] = ambientCy;
			}
			if (ImGui::DragFloat("Z ambient", &ambientCz, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->ambientColor[2] = ambientCz;
			}

			ImGui::Text("Camera");

			float3 cameraPos = App->GetCamera()->GetCameraPos();
			ImGui::InputFloat("X ", &cameraPos.x);
			ImGui::InputFloat("Y ", &cameraPos.y);
			ImGui::InputFloat("Z ", &cameraPos.z);

			std::vector<Mesh*> meshes = focusedObject->getMeshRenderer()->getMeshes();

			for (auto i = 0; i < meshes.size(); i++) {
				ImGui::PushID(i);
				ResourceMaterial* material = meshes[i]->GetMaterial();
				if (ImGui::CollapsingHeader(("Mesh " + std::to_string(i)).c_str())) {

					float shininess = material->GetGlossinessFactor();

					ImGui::Text("Shininess");
					if (ImGui::DragFloat("shininess", &shininess, 0.05f, 0.0f, 0.0f, "%.2f"))
					{
						material->SetGlossinessFactor(shininess);
					}

					float diffuseCx = material->GetDiffuseFactor().x;
					float diffuseCy = material->GetDiffuseFactor().y;
					float diffuseCz = material->GetDiffuseFactor().z;

					ImGui::Text("Diffuse Color");
					if (ImGui::DragFloat("X diffuse", &diffuseCx, 0.05f, 0.0f, 1.0f, "%.2f"))
					{
						material->SetDiffuseFactor(float4(diffuseCx, diffuseCy, diffuseCz, material->GetDiffuseFactor().z));
					}
					if (ImGui::DragFloat("Y diffuse", &diffuseCy, 0.05f, 0.0f, 1.0f, "%.2f"))
					{
						material->SetDiffuseFactor(float4(diffuseCx, diffuseCy, diffuseCz, material->GetDiffuseFactor().z));
					}
					if (ImGui::DragFloat("Z diffuse", &diffuseCz, 0.05f, 0.0f, 0.0f, "%.2f"))
					{
						material->SetDiffuseFactor(float4(diffuseCx, diffuseCy, diffuseCz, material->GetDiffuseFactor().z));
					}

					float specularCx = material->GetSpecularFactor().x;
					float specularCy = material->GetSpecularFactor().y;
					float specularCz = material->GetSpecularFactor().z;

					ImGui::Text("Specular Color");
					if (ImGui::DragFloat("X specular", &specularCx, 0.05f, 0.0f, 1.0f, "%.2f"))
					{
						material->SetSpecularFactor(float3(specularCx, specularCy, specularCz));
					}
					if (ImGui::DragFloat("Y specular", &specularCy, 0.05f, 0.0f, 1.0f, "%.2f"))
					{
						material->SetSpecularFactor(float3(specularCx, specularCy, specularCz));
					}
					if (ImGui::DragFloat("Z specular", &specularCz, 0.05f, 0.0f, 0.0f, "%.2f"))
					{
						material->SetSpecularFactor(float3(specularCx, specularCy, specularCz));
					}

					bool hasDiffuse = material->GetEnableDiffuseTexture();
					bool hasSpecular = material->GetEnableSpecularGlossinessTexture();

					ImGui::Checkbox("Enable Diffuse map", &hasDiffuse);
					ImGui::Checkbox("Enable Specular map", &hasSpecular);

					material->SetEnableDiffuseTexture((int)hasDiffuse);
					material->SetEnableSpecularGlossinessTexture((int)hasSpecular);
				}
				ImGui::PopID();
			}

		}

	}
	ImGui::End();
}