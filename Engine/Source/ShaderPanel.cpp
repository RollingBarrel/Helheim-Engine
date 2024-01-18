#include "ShaderPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "Mesh.h"

#include <MathFunc.h>


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

			static float shininess = focusedObject->getMeshRenderer()->shininess;

			ImGui::Text("Shininess");
			if (ImGui::DragFloat("shininess", &shininess, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->shininess = shininess;
			}

			static float diffuseCx = focusedObject->getMeshRenderer()->diffuseColor[0];
			static float diffuseCy = focusedObject->getMeshRenderer()->diffuseColor[1];
			static float diffuseCz = focusedObject->getMeshRenderer()->diffuseColor[2];

			ImGui::Text("Diffuse Color");
			if (ImGui::DragFloat("X diffuse", &diffuseCx, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->diffuseColor[0] = diffuseCx;
			}
			if (ImGui::DragFloat("Y diffuse", &diffuseCy, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->diffuseColor[1] = diffuseCy;
			}
			if (ImGui::DragFloat("Z diffuse", &diffuseCz, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->diffuseColor[2] = diffuseCz;
			}

			static float specularCx = focusedObject->getMeshRenderer()->specularColor[0];
			static float specularCy = focusedObject->getMeshRenderer()->specularColor[1];
			static float specularCz = focusedObject->getMeshRenderer()->specularColor[2];

			ImGui::Text("Specular Color");
			if (ImGui::DragFloat("X specular", &specularCx, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->specularColor[0] = specularCx;
			}
			if (ImGui::DragFloat("Y specular", &specularCy, 0.05f, 0.0f, 1.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->specularColor[1] = specularCy;
			}
			if (ImGui::DragFloat("Z specular", &specularCz, 0.05f, 0.0f, 0.0f, "%.2f"))
			{
				focusedObject->getMeshRenderer()->specularColor[2] = specularCz;
			}

			static bool hasDiffuse = focusedObject->getMeshRenderer()->hasDiffuseMap;
			static bool hasSpecular = focusedObject->getMeshRenderer()->hasSpecularMap;
			static bool hasShiny = focusedObject->getMeshRenderer()->hasShininessMap;

			ImGui::Checkbox("Diffuse map", &hasDiffuse);
			ImGui::Checkbox("Specular map", &hasSpecular);
			ImGui::Checkbox("Shininess map", &hasShiny);

			focusedObject->getMeshRenderer()->hasDiffuseMap = (int)hasDiffuse;
			focusedObject->getMeshRenderer()->hasSpecularMap = (int)hasSpecular;
			focusedObject->getMeshRenderer()->hasShininessMap = (int)hasShiny;
		}

		ImGui::Text("Camera");

		float3 cameraPos = App->GetCamera()->GetCameraPos();
		ImGui::InputFloat("X ", &cameraPos.x);
		ImGui::InputFloat("Y ", &cameraPos.y);
		ImGui::InputFloat("Z ", &cameraPos.z);
	}
	ImGui::End();
}