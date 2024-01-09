#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "imgui.h"
#include <algorithm>

#include "MeshRendererComponent.h"
#include "TestComponent.h"
#include <MathFunc.h>

GameObject::GameObject(GameObject* parent)
	:mID(LCG().Int()), mName("GameObject"), mParent(parent),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

	AddSuffix();
}

GameObject::GameObject(const GameObject& original)
	:mID(LCG().Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	AddSuffix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const char* name, GameObject* parent)
	:mID(LCG().Int()), mName(name), mParent(parent),
	mIsRoot(parent == nullptr), mIsEnabled(true), mWorldTransformMatrix(float4x4::identity),
	mLocalTransformMatrix(float4x4::identity), mPosition(float3::zero), mScale(float3::one),
	mRotation(float3::zero)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

}

GameObject::~GameObject()
{
	for (GameObject* gameObject : mChildren) {
		delete gameObject;
		gameObject = nullptr;
	}
	mChildren.clear();
	for (Component* component : mComponents) {
		delete component;
		component = nullptr;
	}
	mComponents.clear();

}


/*template<class T>
T* GameObject::GetComponent() {
	T& GameObject::GetComponent() {
		for (auto&& component : components) {
			if (component->IsClassType(T::Type))
				return *static_cast<T*>(component.get());
		}

		return *std::unique_ptr<T>(nullptr);
	}
}
*/
Component* GameObject::GetComponent(ComponentType type)
{
	for (auto component : mComponents) {
		if (component->GetType() == type) {
			return component;
		}
	}
	return nullptr;
}

void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, Quat::FromEulerXYZ(DegToRad(mRotation.x), DegToRad(mRotation.y), DegToRad(mRotation.z)), mScale);

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

	DeleteComponents();
}

void GameObject::ResetTransform()
{
	mPosition = { 0,0,0 };
	mScale = { 1,1,1 };
	mRotation = { 0,0,0 };
}

void GameObject::DeleteChild(GameObject* child)
{
	auto childIterator = std::find(mChildren.begin(), mChildren.end(), child);
	mChildren.erase(childIterator);
	delete child;
	child = nullptr;
}

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}


void GameObject::SetRotation(const float3& rotation)
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

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
		if ((*it)->GetID() == id) {
			movedObject = *it;
			mChildren.erase(it);
			break;
		}
	}
	return movedObject;

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
		if (itMovedObject != mChildren.end()) {
			if (itMovedObject < itTargetPosition) {
				if (itMovedObject + 1 == itTargetPosition) { return; }
				std::rotate(itMovedObject, itMovedObject + 1, itTargetPosition);
			}
			else if (itMovedObject > itTargetPosition) {
				std::rotate(itTargetPosition, itMovedObject, itMovedObject + 1);
			}
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
				if (itMovedObject + 1 == itTargetPosition) { return; }
				std::rotate(itMovedObject, itMovedObject + 1, itTargetPosition);
			}
void GameObject::CreateComponent(ComponentType type) {
	Component* newComponent = nullptr;
			else if (itMovedObject > itTargetPosition) {
	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::TEST:    
			newComponent = new TestComponent(this);
			break;
		default:
			break;
	}
			}
	if (newComponent) {
		mComponents.push_back(newComponent);
	}
}

void GameObject::DeleteComponents() {
	for (auto component : mComponentsToDelete)
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), component);
		if (it != mComponents.end()) {
			mComponents.erase(it);
			delete component;
			component = nullptr;
		}
	}
}

/******************************************************************************
 ***						GUI DRAWING FUNCTIONS							***
 ******************************************************************************/


void GameObject::DragAndDropSource()
{
	else {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
		{
			if ((*it)->GetID() == id)
			{
				newParent->AddChild(*it, aboveThisId);
				mChildren.erase(it);

}

void GameObject::DragAndDropTarget(bool reorder) {
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

void GameObject::AddSuffix()
{
	bool found = true;
	int count = 1;
	int last_pos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		int pos = std::string::npos;

		std::string nameWithSufix = mName + str;
		for (auto gameObject : mParent->mChildren)
		{
			if (pos == -1) {
				//pos = gameObject->mName.find(str, gameObject->mName.size() - 4);
				pos = gameObject->mName.find(nameWithSufix);
			}

		}

		if (pos == std::string::npos) {
			if (mParent->mChildren.size() > 0) {
				mName += str;
			}
			found = false;
		}
		else {
			count++;
			last_pos = pos;
		}

	}
}

void GameObject::CreateComponent(ComponentType type) {
	Component* newComponent = nullptr;

	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::TEST:    
			newComponent = new TestComponent(this);
			break;
		default:
			break;
	}
}


}

void GameObject::DragAndDrop()
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("_TREENODE", this, sizeof(*this));

		ImGui::Text(mName.c_str());
		ImGui::EndDragDropSource();
	}

}

void GameObject::DragAndDropTarget(bool reorder) {
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE"))
		{
			bool isParent = false;
			const GameObject* movedObject = (const GameObject*)payload->Data;

			GameObject* parent = mParent;

			while (parent != nullptr) {
				if (parent->mID == movedObject->mID) {
					isParent = true;
				}
				parent = parent->mParent;
			}

			if (!isParent) {
				GameObject* pMovedObject = movedObject->mParent->RemoveChild(movedObject->GetID());
				if (reorder) { mParent->AddChild(pMovedObject, mID); }
				else { AddChild(pMovedObject); }
			}

		}
		ImGui::EndDragDropTarget();
	}
}

void GameObject::DrawTransform() {
	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
	ImGui::SameLine(ImGui::GetItemRectSize().x - 50.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(4 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(4 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(4 / 7.0f, 0.8f, 0.8f));
	if (ImGui::SmallButton("Config##transform")) {
		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) {
		if (ImGui::Selectable("Reset")) {
	componentIndex = 0;

	for (Component* component : mComponents) {
		component->DrawEditor();
	}

	ImGui::Separator();
	AddComponentButton();
}
	}
	}
			ImGui::PushID(mID);
void GameObject::DrawHierarchy(const int selected)
{
	bool nodeOpen = true;
	if (!mIsRoot) {
		ImGui::Separator();
		DragAndDropTarget(true);
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (mID == selected)
			baseFlags |= ImGuiTreeNodeFlags_Selected;
		if (mChildren.size() == 0) {
			baseFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)mID, baseFlags, mName.c_str()) && (mChildren.size() > 0);
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(this);
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && !ImGui::IsItemToggledOpen()) {
			App->GetScene()->SetSelectedObject(this);
		}
		OnRightClick();
		DragAndDropSource();
	}
	else {
		nodeOpen = ImGui::CollapsingHeader(mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
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
			ImGui::PushID(mID + 1);
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
			ImGui::PushID(mID + 2);
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

void GameObject::AddComponentButton() {
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	ImGui::SetCursorPosX(posX);

	bool hasMeshRendererComponent = false;
	bool hasMaterialComponent = false;

	for (Component* component : mComponents) {
		if (component->GetType() == ComponentType::MESHRENDERER) {
			hasMeshRendererComponent = true;
		}
		else if (component->GetType() == ComponentType::TEST) {
			hasMaterialComponent = true;
		}

		if (hasMeshRendererComponent && hasMaterialComponent) {
			break;
		}
	}

	if (!hasMeshRendererComponent || !hasMaterialComponent) {
		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
			ImGui::OpenPopup("AddComponentPopup");
		}
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (!hasMeshRendererComponent && ImGui::MenuItem("Mesh Renderer")) {
			CreateComponent(ComponentType::MESHRENDERER);
		}
		if (!hasMaterialComponent && ImGui::MenuItem("Test")) {
			CreateComponent(ComponentType::TEST);
		}

		ImGui::EndPopup();
	}
}

void GameObject::DrawInspector() {
	char nameArray[100];
	strcpy_s(nameArray, mName.c_str());
	ImGui::PushID(mID);
	ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
	ImGui::PopID();
	mName = nameArray;
	DrawTransform();

	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::TEST:    
			newComponent = new TestComponent(this);
			break;
		default:
			break;
	}

	if (newComponent) {
		mComponents.push_back(newComponent);
	}
	DragAndDropTarget();
	if (nodeOpen) {
		for (auto child : mChildren) {
			child->DrawHierarchy(selected);
		}

		if (!mIsRoot) {
			ImGui::TreePop();
		}
	}
	if (mIsRoot) {
		ImGui::Separator();
		DragAndDropTarget();
	}

}



void GameObject::OnRightClick() {
	ImGui::PushID(mID);
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("OptionsGO");
	}
	if (ImGui::BeginPopup("OptionsGO")) {
		if (ImGui::Selectable("Create GameObject")) {
			GameObject* gameObject = new GameObject(this);
			AddChild(gameObject);
			App->GetScene()->SetSelectedObject(gameObject);
		}

		if (!mIsRoot) {
			if (ImGui::Selectable("Duplicate")) {
				GameObject* gameObject = new GameObject(*this);
				//mParent->AddChild(gameObject);
				App->GetScene()->AddGameObjectToDuplicate(gameObject);
				App->GetScene()->SetSelectedObject(gameObject);
			}
		}

		if (!mIsRoot) {
			if (ImGui::Selectable("Delete")) {
				App->GetScene()->AddGameObjectToDelete(this);
				App->GetScene()->SetSelectedObject(App->GetScene()->GetRoot());
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}