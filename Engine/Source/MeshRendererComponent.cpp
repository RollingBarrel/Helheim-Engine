#include "MeshRendererComponent.h"
#include "imgui.h"

MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" , owner, ComponentType::MESHRENDERER)
{

}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(original.mName , owner, ComponentType::MESHRENDERER)
{

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
	else {
		RightClickPopup();
	}
}

Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
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


