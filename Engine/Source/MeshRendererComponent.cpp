#include "MeshRendererComponent.h"
#include "imgui.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
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
	float rv1 = distribution(gen)/10;
	float rv2 = distribution(gen)/10;
	float rv3 = distribution(gen)/10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1+1.0f, rv2 + 1.0f, rv3 + 1.0f)));
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
	float rv1 = distribution(gen) / 10;
	float rv2 = distribution(gen) / 10;
	float rv3 = distribution(gen) / 10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1 + 1.0f, rv2 + 1.0f, rv3 + 1.0f)));
}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum)
	{
		return;
	}
	if(mDrawBox)
		App->GetDebugDraw()->DrawBoundingBox(mOBB);

	mInsideFrustum = false;
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
		ImGui::Checkbox("Render bounding box", &mDrawBox);
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