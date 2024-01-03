#include "MeshRendererComponent.h"
#include "imgui.h"


MeshRendererComponent::MeshRendererComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::MESHRENDERER)
{
	
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original)
	:Component(original.GetOwner(), ComponentType::MESHRENDERER)
{
}

void MeshRendererComponent::Draw()
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
	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Model: Cube.obj (TEST)");
		ImGui::Text("Material: DefaultMaterial (TEST)");
		ImGui::Text("Shader: StandardShader (TEST)");
	}

	if (gameObject) {
		gameObject->DeletePopup(this, 55);
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