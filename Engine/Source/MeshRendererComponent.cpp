#include "MeshRendererComponent.h"
#include "imgui.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"

#include <iostream>
#include <random>

MeshRendererComponent::MeshRendererComponent(GameObject* ownerGameObject) 
	:Component(ownerGameObject, ComponentType::MESHRENDERER)
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-100, 90);

	// Generate three random values
	int rv1 = distribution(gen);
	int rv2 = distribution(gen);
	int rv3 = distribution(gen);
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1+10, rv2+10, rv3+10)));
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original)
	:Component(original.GetOwner(), ComponentType::MESHRENDERER)
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-100, 90);

	// Generate three random values
	int rv1 = distribution(gen);
	int rv2 = distribution(gen);
	int rv3 = distribution(gen);
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1+10, rv2+10, rv3+10)));
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
		ImGui::DragFloat3("Bounding Box (position): ", &mOBB.pos.x);
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