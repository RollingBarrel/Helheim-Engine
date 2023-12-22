#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "MathGeoLib.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "imgui.h"

GameObject::GameObject(const GameObject* parent) 
	:mID(2), mName("GameObject"), mParent(parent),
	mIsRoot(false), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{
	if (parent == nullptr) {
		mIsRoot = true;
	}
	else {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}
}

GameObject::GameObject(const char* name, const GameObject* parent)
	:mID(2), mName(name), mParent(parent),
	mIsRoot(false), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{

	


}

void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);

	mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->RecalculateMatrices();
	}

}

void GameObject::Update()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		mComponents[i]->Update();
	}

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->Update();
	}
}

void GameObject::SetRotation(const Quat& rotation)
{
	mRotation = rotation;
	RecalculateMatrices();
}

void GameObject::SetPosition(const float3& position)
{
	mPosition = position;
	RecalculateMatrices();
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;
	RecalculateMatrices();
}



void GameObject::DrawInspector() {
	DrawTransform();

	for (Component* component : mComponents) {
		ImGui::Separator();
		component->DrawEditor();
	}

}

void GameObject::DrawHierarchy()
{
	for (auto child : mChildren) {
		child->DrawHierarchy();
	}
}

void GameObject::AddChild(GameObject& child)
{
	mChildren.push_back(&child);
}

void GameObject::DrawTransform() {
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		bool modifiedTransform = false;
		if (ImGui::BeginTable("transformTable", 2)) {
			ImGui::TableNextRow();
			ImGui::PushID(0);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Position");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mPosition.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mPosition.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mPosition.z);
			ImGui::PopItemWidth();
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::PushID(1);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Rotation");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mRotation.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mRotation.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mRotation.z);
			ImGui::PopItemWidth();
			ImGui::PopID();

			ImGui::TableNextRow();
			ImGui::PushID(2);
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Scale");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth(1) / 4);
			modifiedTransform = modifiedTransform || ImGui::InputFloat("X", &mScale.x);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Y", &mScale.y);
			ImGui::SameLine();
			modifiedTransform = modifiedTransform || ImGui::InputFloat("Z", &mScale.z);
			ImGui::PopItemWidth();
			ImGui::PopID();
			if (modifiedTransform) {
				RecalculateMatrices();
			}
		}
		ImGui::EndTable();
	}
}