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
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

	AddSufix();
}

GameObject::GameObject(const GameObject& original)
	:mID((new LCG())->Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	AddSufix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}
	//TODO: Copy Childs and Components
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID((new LCG())->Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	//AddSufix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}
	//TODO: Copy Childs and Components
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

	//*************************************************************************************
	componentIndex = 0; // Generate an unique identifier for each component

	for (Component* component : mComponents) {
		component->DrawEditor();

		// Show all the Components created for this GameObject
		ShowComponents(component);
	}

	ImGui::Separator();
	AddComponentButton();

	//**********************************************************************************************************
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
		nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)mID, baseFlags, mName.c_str()) && (mChildren.size() > 0);
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

void GameObject::AddSufix()
{
	bool found = true;
	int count = 1;
	int last_pos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		int pos = std::string::npos;
		for (auto gameObject : mParent->mChildren)
		{
			if (pos == -1) {
				//pos = gameObject->mName.find(str, gameObject->mName.size() - 4);
				pos = gameObject->mName.find(mName + str);
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

//**************************************************************************************************************

void GameObject::AddComponent(Component* component) {
	mComponents.push_back(component);
}

void GameObject::RemoveComponent(Component* component) {
	auto it = std::find(mComponents.begin(), mComponents.end(), component);
	if (it != mComponents.end()) {
		mComponents.erase(it);
		Component::DeleteComponent(component); // Eliminar el componente completamente
	}
}

/*
void GameObject::RemoveComponent(Component* component) {
	auto it = std::find(mComponents.begin(), mComponents.end(), component);
	if (it != mComponents.end()) {
		Component::DeleteComponent(*it); // Eliminar el componente completamente
		mComponents.erase(it); // Eliminar el componente de la lista de componentes del GameObject
	}
}
*/

void GameObject::AddComponentButton() {
	// Calculate the width of the current ImGui window
	float windowWidth = ImGui::GetWindowWidth();

	// Calculate the X position to center the button
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	// Sets the X position of the cursor to center the button
	ImGui::SetCursorPosX(posX);

	// Draw the button centered
	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
		//Popup Menu
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (ImGui::MenuItem("Mesh Renderer")) {
			Component* newComponent = Component::CreateComponent(ComponentType::MESHRENDERER, this);
		}
		if (ImGui::MenuItem("Material")) {
			Component* newComponent = Component::CreateComponent(ComponentType::MATERIAL, this);
		}

		ImGui::EndPopup();
	}
}

void GameObject::ShowComponents(Component* component) {
	if (!mComponents.empty()) { // Check if mComponents vector is not empty
		ImGui::Separator();
		switch (component->GetType()) {
		case ComponentType::MESHRENDERER:
			DrawMeshRenderer(component);
			break;
		case ComponentType::MATERIAL:
			DrawMaterial(component);
			break;
		default:
			break;
		}
	}
}

void GameObject::DrawMeshRenderer(Component* component) {
	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Model: Cube.obj (TEST)"); componentLines++;
		ImGui::Text("Material: DefaultMaterial (TEST)"); componentLines++;
		ImGui::Text("Shader: StandardShader (TEST)"); componentLines++;
	}

	// Activate Delete Popup for every Component
	DeletePopup(component);
}

void GameObject::DrawMaterial(Component* component) {
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
		// SIMULATED CONTENT FOR TEST PURPOSES:
		ImGui::Text("Color: (R: 255, G: 0, B: 0) (TEST)"); componentLines++;
		ImGui::Text("Texture: DefaultTexture (TEST)"); componentLines++;
	}

	// Activate Delete Popup for every Component
	DeletePopup(component);
}

void GameObject::DeletePopup(Component* component) {
	// Calculate the header position using the number of text lines of the component
	float headerPosition = componentLines * 13.5;
	componentLines = 1; // Reset to start by counting the header

	// Create a unique identifier for the Delete Popup
	ImGui::PushID(componentIndex);
	std::string popupID = "ComponentOptions_" + std::to_string(componentIndex);

	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();

	// Set header position detection
	min.y -= ImGui::GetStyle().FramePadding.y + headerPosition;
	max.y += ImGui::GetStyle().FramePadding.y - headerPosition;

	// Open Delete Popup with righ click when header is open
	if (ImGui::IsMouseHoveringRect(min, max) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(popupID.c_str());
	}
	
	// Open Delete Popup with right click when header is close
	if (ImGui::BeginPopupContextItem(popupID.c_str())) {
		ImGui::OpenPopup(popupID.c_str());
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(popupID.c_str())) {
		if (ImGui::MenuItem("Delete Component")) {
			// Delete currently selected component
			RemoveComponent(component);
			ImGui::CloseCurrentPopup(); // Close popup menu after deleting
		}
		ImGui::EndPopup();
	}

	ImGui::PopID(); // Reset ID for next component
	componentIndex++;
}

//********************************************************************************************