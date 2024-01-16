#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "InspectorPanel.h"
#include "Quadtree.h"
#include "imgui.h"
#include <algorithm>

#include "MeshRendererComponent.h"
#include "TestComponent.h"


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
	mLocalTransformMatrix(original.mLocalTransformMatrix)

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
	mLocalTransformMatrix(original.mLocalTransformMatrix)
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
	mIsRoot(parent == nullptr)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

}

GameObject::GameObject(const char* name, unsigned int id, GameObject* parent, float3 position, float3 scale, Quat rotation)
	:mID(id), mName(name), mParent(parent), mPosition(position),
	mScale(scale), mRotation(rotation), mIsRoot(parent == nullptr)
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;
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
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);

	mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->RecalculateMatrices();
	}

	isTransformModified = false;

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
	if (isTransformModified) {
		RecalculateMatrices();
	}

}

void GameObject::ResetTransform()
{
	SetPosition(float3::zero);
	SetRotation(float3::zero);
	SetScale(float3::one);
}

void GameObject::DeleteChild(GameObject* child)
{
	
	
	RemoveChild(child->mID);
	delete child;
	child = nullptr;
}

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}

void GameObject::SetRotation(const float3& rotationInRadians)
{

	Quat deltaRotation = Quat::FromEulerXYZ(rotationInRadians.x - mEulerRotation.x , rotationInRadians.y - mEulerRotation.y, rotationInRadians.z - mEulerRotation.z);
	mRotation = mRotation * deltaRotation;
	mEulerRotation = rotationInRadians;

	isTransformModified = true;
}

void GameObject::SetRotation(const Quat& rotation)
{

	mRotation = rotation;

}

void GameObject::SetPosition(const float3& position)
{
	mPosition = position;
	isTransformModified = true;
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;

	isTransformModified = true;
}

void GameObject::SetID(const unsigned int ID)
{
	mID = ID;
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

	child->RecalculateLocalTransform();

	if (!inserted) {
		mChildren.push_back(child);
	}
	if (child->getMeshRenderer() != nullptr) {
		App->GetScene()->GetQuadtreeRoot()->AddObject(child);
	}
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
		if ((*it)->GetID() == id) {
			if ((*it)->getMeshRenderer() != nullptr) {
				App->GetScene()->GetQuadtreeRoot()->RemoveObject((*it));
			}
			movedObject = *it;
			mChildren.erase(it);
			break;
		}
	}
	return movedObject;

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

Component* GameObject::CreateComponent(ComponentType type) {
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

	if (newComponent) {
		mComponents.push_back(newComponent);
	}
	if (type == ComponentType::MESHRENDERER)
	{
		App->GetScene()->GetQuadtreeRoot()->AddObject(this);
	}
	
	return newComponent;
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

MeshRendererComponent* GameObject::getMeshRenderer() const
{
	for (const auto& comp : mComponents) {
		if (comp->GetType() == ComponentType::MESHRENDERER)
			return static_cast<MeshRendererComponent*>(comp);
	}
}


void GameObject::RecalculateLocalTransform()
{

	mLocalTransformMatrix = mParent->mWorldTransformMatrix.Inverted().Mul(mWorldTransformMatrix);

	mLocalTransformMatrix.Decompose(mPosition, mRotation, mScale);
	mEulerRotation = mRotation.ToEulerXYZ();

	if (mEulerRotation.Equals(float3::zero)) {
		mEulerRotation = float3::zero;
	}
}

void GameObject::Save(Archive& archive) const {
	archive.AddString("name", mName);
	archive.AddBool("isEnabled", mIsEnabled);
	archive.AddFloat3("position", mPosition);
	archive.AddFloat3("scale", mScale);

	// Save components
	Archive* componentsArchive = new Archive();
	for (const auto& component : mComponents) {
		component->Save(*componentsArchive);
	}
	archive.AddObject("components", *componentsArchive);

	// Save children
	Archive* childrenArchive = new Archive();
	for (const auto& child : mChildren) {
		Archive* childArchive = new Archive();
		child->Save(*childArchive);
		childrenArchive->AddObject("child", *childArchive);
	}
	archive.AddObject("children", *childrenArchive);
}

GameObject* findGameObjectParent(GameObject* gameObject, int UID) {
	const std::vector<GameObject*>& gameObjects = gameObject->GetChildren();
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetID() == UID) {
			return gameObjects[i];
		}
		else if (gameObjects[i]->GetChildren().size() != 0) {
			for (int j = 0; j < gameObjects[i]->GetChildren().size(); j++) {
				findGameObjectParent(gameObjects[i]->GetChildren()[j], UID);
			}

		}
	}

	return nullptr;
}

void loadComponentsFromJSON(const rapidjson::Value& components, GameObject* go) {
	for (rapidjson::SizeType i = 0; i < components.Size(); i++) {
		if (components[i].IsObject()) {
			const rapidjson::Value& component = components[i];
			if (component.HasMember("ComponentType") && component["ComponentType"].IsInt()) {
				ComponentType cType = ComponentType(component["ComponentType"].GetInt());
				Component* c = go->CreateComponent(cType);
				c->Load(component);
			}
		}
	}
}

void loadGameObjectFromJSON(const rapidjson::Value& gameObject, GameObject* scene) {
	unsigned int uuid{ 0 };
	int parentUID{ 0 };
	const char* name = { "" };
	float3 position;
	float3 scale;
	Quat rotation;
	if (gameObject.HasMember("UID") && gameObject["UID"].IsInt()) {
		uuid = gameObject["UID"].GetInt();
	}
	if (gameObject.HasMember("ParentUID") && gameObject["ParentUID"].IsInt()) {
		parentUID = gameObject["ParentUID"].GetInt();
	}
	if (gameObject.HasMember("Name") && gameObject["Name"].IsString()) {
		name = gameObject["Name"].GetString();
	}
	if (gameObject.HasMember("Translation") && gameObject["Translation"].IsArray()) {
		const rapidjson::Value& translationValues = gameObject["Translation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (translationValues.Size() == 3 && translationValues[0].IsInt() && translationValues[1].IsInt() && translationValues[2].IsInt()) {
			x = translationValues[0].GetInt();
			y = translationValues[1].GetInt();
			z = translationValues[2].GetInt();
		}

		position = float3(x, y, z);
	}
	if (gameObject.HasMember("Rotation") && gameObject["Rotation"].IsArray()) {
		const rapidjson::Value& rotationValues = gameObject["Rotation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
		if (rotationValues.Size() == 4 && rotationValues[0].IsInt() && rotationValues[1].IsInt() && rotationValues[2].IsInt() && rotationValues[3].IsInt()) {
			x = rotationValues[0].GetInt();
			y = rotationValues[1].GetInt();
			z = rotationValues[2].GetInt();
			w = rotationValues[3].GetInt();
		}

		rotation = Quat(x, y, z, w);
	}

	if (gameObject.HasMember("Scale") && gameObject["Scale"].IsArray()) {
		const rapidjson::Value& scaleValues = gameObject["Scale"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (scaleValues.Size() == 3 && scaleValues[0].IsInt() && scaleValues[1].IsInt() && scaleValues[2].IsInt()) {
			x = scaleValues[0].GetInt();
			y = scaleValues[1].GetInt();
			z = scaleValues[2].GetInt();
		}

		scale = float3(x, y, z);
	}

	GameObject* go;

	if (parentUID == 0) {
		go = new GameObject(name, uuid, scene, position, scale, rotation);
		// Manage Components
		if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
			loadComponentsFromJSON(gameObject["Components"], go);
		}

		scene->AddChild(go, parentUID);
	}
	else {
		GameObject* gameObjectParent = findGameObjectParent(scene, parentUID);
		go = new GameObject(name, uuid, gameObjectParent, position, scale, rotation);
		// Manage Components
		if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
			loadComponentsFromJSON(gameObject["Components"], go);
		}

		gameObjectParent->AddChild(go);

	}
}

void GameObject::Load(const rapidjson::Value& gameObjectsJson) {
	GameObject* scene = App->GetScene()->GetRoot();
	// Manage GameObjects inside the Scene
	if (gameObjectsJson.HasMember("GameObjects") && gameObjectsJson["GameObjects"].IsArray()) {
		const rapidjson::Value& gameObjects = gameObjectsJson["GameObjects"];
		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++) {
			if (gameObjects[i].IsObject()) {
				loadGameObjectFromJSON(gameObjects[i], scene);
			}
		}
	}
}


void GameObject::RemoveComponent(Component* component) {
	auto it = std::find(mComponents.begin(), mComponents.end(), component);
	if (it != mComponents.end()) {
		mComponents.erase(it);
		delete component;
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

void GameObject::DragAndDrop()
{
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
			ResetTransform();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor(3);
	if (headerOpen) {
		bool modifiedTransform = false;
		if (ImGui::BeginTable("transformTable", 2)) {
			ImGui::TableNextRow();
			ImGui::PushID(mID);
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

