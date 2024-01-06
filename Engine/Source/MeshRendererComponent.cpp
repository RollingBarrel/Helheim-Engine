#include "MeshRendererComponent.h"
#include "imgui.h"

MeshRendererComponent::MeshRendererComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::MESHRENDERER)
{
	mName = "MeshRenderer";
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original)
	:Component(original.GetOwner(), ComponentType::MESHRENDERER)
{
	mName = "MeshRenderer";
}

void MeshRendererComponent::Draw()
{

}
void MeshRendererComponent::Reset()
{

}
void MeshRendererComponent::Load()
{
	LoadVBO();
}

void MeshRendererComponent::Update()
{
	Draw();
}

void MeshRendererComponent::DrawEditor()
{

	if (IsComponentOpen()) {
		RightClickPopup();
		ImGui::Text("Model: Cube.obj (TEST)");
		ImGui::Text("Material: DefaultMaterial (TEST)");
		ImGui::Text("Shader: StandardShader (TEST)");
	}
}

Component* MeshRendererComponent::Clone()
{
	return new MeshRendererComponent(*this);
}

void MeshRendererComponent::LoadVBO()
{

}

void MeshRendererComponent::LoadEBO()
{
}

void MeshRendererComponent::LoadVAO()
{
}

void MeshRendererComponent::RightClickPopup()
{
	Component::RightClickPopup();
	
	if (ImGui::BeginPopup(mPopupID)) {
		if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


}


