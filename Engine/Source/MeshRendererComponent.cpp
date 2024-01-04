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
	bool headerOpen = ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth |ImGuiTreeNodeFlags_AllowItemOverlap);
	ImGui::SameLine(ImGui::GetItemRectSize().x - 50.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
	if (ImGui::SmallButton("Config##MeshRenderer")) {
		ImGui::OpenPopup("MeshRendererOptions");
	}
	if (ImGui::BeginPopup("MeshRendererOptions")) {
		if (ImGui::Selectable("Reset")) {
			Reset();
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor(3);
	if (headerOpen) {
		ImGui::Text("Model: Cube.obj (TEST)");
		ImGui::Text("Material: DefaultMaterial (TEST)");
		ImGui::Text("Shader: StandardShader (TEST)");
	}

	if (this->GetOwner()) {
		this->GetOwner()->DeletePopup(this, 55);
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