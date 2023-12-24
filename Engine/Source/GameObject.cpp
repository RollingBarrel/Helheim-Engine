#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "imgui.h"

GameObject::GameObject(GameObject* parent) 
	:mID((new LCG())->Int()), mName("GameObject"), mParent(parent),
	mIsRoot(parent == nullptr), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{
	if(!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}
}

GameObject::GameObject(const char* name, GameObject* parent)
	:mID((new LCG())->Int()), mName(name), mParent(parent),
	mIsRoot(parent == nullptr), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero),
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero),
	mRotation(Quat::identity)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

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
	
	ImGui::Text(mName.c_str());
	DrawTransform();

	for (Component* component : mComponents) {
		ImGui::Separator();
		component->DrawEditor();
	}

}

void GameObject::DrawHierarchy(const int selected)
{
	bool nodeOpen = true;
	if (!mIsRoot) {
		ImGui::Separator();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this->mParent, mID);
			}
			ImGui::EndDragDropTarget();
		}
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (mID == selected)
			baseFlags |= ImGuiTreeNodeFlags_Selected;
		if (mChildren.size() == 0) {
			baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)mID, baseFlags, "%s %d", mName.c_str(), mID) && (mChildren.size() > 0);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(this);
		}
		/*****Begin Drag & Drop Code*******/
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			ImGui::SetDragDropPayload("_TREENODE", this, sizeof(*this));

			ImGui::Text(mName.c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this);
			}
			ImGui::EndDragDropTarget();
		}
		/*****End Drag & Drop Code*******/
	}
	
	if (nodeOpen) {
		for (auto child : mChildren) {
			child->DrawHierarchy(selected);
		}
	
		if (!mIsRoot) {
			ImGui::TreePop(); 
		}
	}
	if (mIsRoot) { // Dragging something to this Separator will move it to the end of root
		ImGui::Separator();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
			{
				const GameObject* movedObject = (const GameObject*)payload->Data;
				movedObject->mParent->MoveChild(movedObject->GetID(), this);
			}
			ImGui::EndDragDropTarget();
		}
	}
}

void GameObject::AddChild(GameObject* child, const int aboveThisId)
{
	child->mParent = this;
	bool inserted = false;
	if (aboveThisId != 0) {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it) {
			if ((*it)->GetID() == aboveThisId)
			{				
				mChildren.insert(it, child);
				inserted = true;
				break;
			}
		}
	}
	if (!inserted) {
		mChildren.push_back(child);
	}
	
}

void GameObject::MoveChild(const int id, GameObject* newParent, const int aboveThisId)
{
	if (mID == newParent->mID) { //Movement inside same object
		if (id == aboveThisId) { return; }
		std::vector<GameObject*>::iterator itMovedObject;
		std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
		for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
			if ((*it)->GetID() == id) {
				itMovedObject = it;
			}
			else if ((*it)->GetID() == aboveThisId) {
				itTargetPosition = it;
			}
		}
		if (itMovedObject != mChildren.end()) {
			if (itMovedObject < itTargetPosition) {
				if (itMovedObject + 1 == itTargetPosition) { return; }
				std::rotate(itMovedObject, itMovedObject + 1, itTargetPosition);
			}
			else if (itMovedObject > itTargetPosition) {
				std::rotate(itTargetPosition, itMovedObject, itMovedObject + 1);
			}
		}
		
	}
	else {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
		{
			if ((*it)->GetID() == id)
			{
				newParent->AddChild(*it, aboveThisId);
				mChildren.erase(it);
				break;
			}
		}
	}
			
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