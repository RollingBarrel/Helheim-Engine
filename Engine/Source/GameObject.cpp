#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include "InspectorPanel.h"
#include "Quadtree.h"
#include "imgui.h"
#include "ModuleOpenGL.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include <algorithm>
#include "MathFunc.h"

#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "TestComponent.h"
#include "NavMeshControllerComponent.h"
#include "TagsManager.h"
#include "Tag.h"
GameObject::GameObject(GameObject* parent)
	:mID(LCG().Int()), mName("GameObject"), mParent(parent),mTag(App->GetTags()->GetTagByName("Untagged")),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
	}
	AddSuffix();

}

GameObject::GameObject(const GameObject& original)
	:mID(LCG().Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(original.mIsActive),
	mWorldTransformMatrix(original.mWorldTransformMatrix), mLocalTransformMatrix(original.mLocalTransformMatrix), mTag(original.GetTag())
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
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(newParent->mIsActive),
	mWorldTransformMatrix(original.mWorldTransformMatrix), mLocalTransformMatrix(original.mLocalTransformMatrix),
	mTag(original.GetTag())
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
	:mID(LCG().Int()), mName(name), mParent(parent), mTag(App->GetTags()->GetTagByName("Untagged")),
	mIsRoot(parent == nullptr)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
	}
}

GameObject::GameObject(const char* name, unsigned int id, GameObject* parent, float3 position, float3 scale, Quat rotation)
	:mID(id), mName(name), mParent(parent), mPosition(position),
	mScale(scale), mRotation(rotation), mIsRoot(parent == nullptr),
	mTag(App->GetTags()->GetTagByName("Untagged"))
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
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
	if (IsActive())
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
			RefreshBoundingBoxes();
		}
	}
}

void GameObject::ResetTransform()
{
	SetPosition(float3::zero);
	SetRotation(float3::zero);
	SetScale(float3::one);

	if (getCamera() != nullptr) {
		CameraComponent* camera = getCamera();
		camera->Reset();
	}
}

void GameObject::SetEnabled(bool enabled)
{
	mIsEnabled = enabled;

	if (!enabled || mParent->IsActive())
	{
		SetActiveInHierarchy(enabled);
	}
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
	float3 difference = rotationInRadians - mEulerRotation;
	Quat deltaRotation = Quat::FromEulerXYZ(rotationInRadians.x - mEulerRotation.x , rotationInRadians.y - mEulerRotation.y, rotationInRadians.z - mEulerRotation.z);
	mRotation = mRotation * deltaRotation;
	mEulerRotation = rotationInRadians;

	if (getCamera() != nullptr) {
		CameraComponent* camera = getCamera();
		camera->SetRotation(difference);
	}

	isTransformModified = true;
}

void GameObject::SetRotation(const Quat& rotation)
{
	mRotation = rotation;
	mEulerRotation = rotation.ToEulerXYZ();

	isTransformModified = true;
}

void GameObject::SetPosition(const float3& position)
{
	float3 difference = position - mPosition;

	mPosition = position;

	isTransformModified = true;

	if (getCamera() != nullptr) {
		CameraComponent* camera = getCamera();
		camera->SetPosition(difference);
	}
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;

	isTransformModified = true;
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
	if (child->GetComponent(ComponentType::MESHRENDERER) != nullptr) {
		App->GetScene()->GetQuadtreeRoot()->AddObject(child);
	}
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
		if ((*it)->GetID() == id) {
			if ((*it)->GetComponent(ComponentType::MESHRENDERER) != nullptr) {
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
	size_t lastPos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		size_t pos = std::string::npos;

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
			lastPos = pos;
		}

	}
}

//TODO: Crate a component that requires ids not clean now
Component* GameObject::CreateComponent(ComponentType type, unsigned int meshUid, unsigned int materialUid) {
	Component* newComponent = nullptr;

	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this, meshUid, materialUid);
			break;
		case ComponentType::CAMERA:
			newComponent = new CameraComponent(this);
			break;
		case ComponentType::POINTLIGHT:
		{
			const float3& pos = GetWorldPosition();
			newComponent = App->GetOpenGL()->AddPointLight({ pos.x, pos.y, pos.z, 1.f, 1.f, 1.f, 3.f }, this);
			break;
		}
		case ComponentType::SPOTLIGHT:
		{
			const float3& pos = GetWorldPosition();
			newComponent = App->GetOpenGL()->AddSpotLight({ 3.f , 0.0f, 0.0f, 0.0f, pos.x, pos.y, pos.z, 1.5f, 0.f, -1.f, 0.f, cos(DegToRad(25.f)), 1.f, 1.f, 1.f , cos(DegToRad(38.f))}, this);
			break;
		}
		case ComponentType::TEST:
			newComponent = new TestComponent(this);
			break;
		case ComponentType::NAVMESHCONTROLLER:
			newComponent = new NavMeshControllerComponent(this);
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

Component* GameObject::RemoveComponent(Component* component)
{
	Component* movedComponent = nullptr;
	for (auto it = mComponents.begin(); it != mComponents.cend(); ++it) {
		if ((*it)->GetID() == component->GetID()) {
			movedComponent = *it;
			mComponents.erase(it);
			break;
		}
	}
	return movedComponent;
}

void GameObject::AddComponent(Component* component, Component* position)
{
	if (position == nullptr) {
		mComponents.push_back(component);
	}
	else {
		auto it = std::find(mComponents.begin(), mComponents.end(), position);
		mComponents.insert(it, component);
	}
}

MeshRendererComponent* GameObject::GetMeshRenderer() const
{
	auto it = std::find_if(mComponents.begin(), mComponents.end(), [](const Component* comp) {
		return comp->GetType() == ComponentType::MESHRENDERER;
		});

	if (it != mComponents.end()) {
		return static_cast<MeshRendererComponent*>(*it);
	}

	return nullptr;
}

CameraComponent* GameObject::getCamera() const
{
	auto it = std::find_if(mComponents.begin(), mComponents.end(), [](const Component* comp) {
		return comp->GetType() == ComponentType::CAMERA;
		});

	if (it != mComponents.end()) {
		return static_cast<CameraComponent*>(*it);
	}

	return nullptr;
}

void GameObject::RecalculateLocalTransform() {

	mLocalTransformMatrix = mParent->mWorldTransformMatrix.Inverted().Mul(mWorldTransformMatrix);

	mLocalTransformMatrix.Decompose(mPosition, mRotation, mScale);
	mEulerRotation = mRotation.ToEulerXYZ();

	if (mEulerRotation.Equals(float3::zero)) {
		mEulerRotation = float3::zero;
	}
}

void GameObject::RefreshBoundingBoxes()
{
	if (GetMeshRenderer() != nullptr)
	{
		GetMeshRenderer()->RefreshBoundingBoxes();
	}
	else
	{
		for (auto children : mChildren)
		{
			children->RefreshBoundingBoxes();
		}
	}
}

void GameObject::SetActiveInHierarchy(bool active)
{
	if (active && !mIsEnabled)
	{
		return;
	}

	mIsActive = active;

	for (GameObject* child : mChildren)
	{
		child->SetActiveInHierarchy(active);
	}
}

void GameObject::Save(Archive& archive) const {
	archive.AddInt("UID", mID);
	archive.AddInt("ParentUID", mParent->GetID());
	archive.AddString("Name", mName.c_str());
	archive.AddBool("isEnabled", mIsEnabled);
	archive.AddBool("isActive", mIsActive);
	archive.AddFloat3("Translation", mPosition);
	archive.AddQuat("Rotation", mRotation);
	archive.AddFloat3("Scale", mScale);
	archive.AddInt("Tag", mTag->GetID());

	// Save components
	std::vector<Archive> componentsArchiveVector;

	for (const auto& component : mComponents) {
		Archive componentArchive;
		component->Save(componentArchive);
		componentsArchiveVector.push_back(componentArchive);
	}

	archive.AddObjectArray("Components", componentsArchiveVector);

	// Save children IDs
	/*if (!mChildren.empty()) {
		std::vector<unsigned int> childrenIds;
		for (const auto& child : mChildren) {
			childrenIds.push_back(child->GetId());
		}
		archive.AddIntArray("Children", childrenIds);
	}*/
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
				c->LoadFromJSON(component, go);
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
	Tag* tag = nullptr;

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
		if (translationValues.Size() == 3 && translationValues[0].IsFloat() && translationValues[1].IsFloat() && translationValues[2].IsFloat()) {
			x = translationValues[0].GetFloat();
			y = translationValues[1].GetFloat();
			z = translationValues[2].GetFloat();
		}

		position = float3(x, y, z);
	}
	if (gameObject.HasMember("Rotation") && gameObject["Rotation"].IsArray()) {
		const rapidjson::Value& rotationValues = gameObject["Rotation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
		if (rotationValues.Size() == 4 && rotationValues[0].IsFloat() && rotationValues[1].IsFloat() && rotationValues[2].IsFloat() && rotationValues[3].IsFloat()) {
			x = rotationValues[0].GetFloat();
			y = rotationValues[1].GetFloat();
			z = rotationValues[2].GetFloat();
			w = rotationValues[3].GetFloat();
		}

		rotation = Quat(x, y, z, w);
	}

	if (gameObject.HasMember("Scale") && gameObject["Scale"].IsArray()) {
		const rapidjson::Value& scaleValues = gameObject["Scale"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (scaleValues.Size() == 3 && scaleValues[0].IsFloat() && scaleValues[1].IsFloat() && scaleValues[2].IsFloat()) {
			x = scaleValues[0].GetFloat();
			y = scaleValues[1].GetFloat();
			z = scaleValues[2].GetFloat();
		}

		scale = float3(x, y, z);
	}

	if (gameObject.HasMember("Tag")) {
		const rapidjson::Value& tagint = gameObject["Tag"];
		int tagid = tagint[0].GetInt();
		Tag* loadedTag = App->GetTags()->GetTagByID(tagid);

		if (loadedTag == nullptr) {
			tag = App->GetTags()->GetTagByName("Untagged");
		}
		else {
			tag = loadedTag;
		}

	}

	GameObject* go;

	if (parentUID == 1) {
		go = new GameObject(name, uuid, scene, position, scale, rotation);
		// Manage Components
		if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
			loadComponentsFromJSON(gameObject["Components"], go);
		}
	}
	else {
		GameObject* gameObjectParent = findGameObjectParent(scene, parentUID);
		go = new GameObject(name, uuid, gameObjectParent, position, scale, rotation);
		// Manage Components
		if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
			loadComponentsFromJSON(gameObject["Components"], go);
		}
	}

	go->SetTag(tag);
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

GameObject* GameObject::FindGameObjectWithTag(std::string tagname)

{
	GameObject* foundGameObject = App->GetScene()->FindGameObjectWithTag(tagname);
	if (foundGameObject != nullptr) {
		return foundGameObject;
	}
	return nullptr;
}

std::vector<GameObject*> GameObject::FindGameObjectsWithTag(std::string tagname)
{
	return App->GetScene()->FindGameObjectsWithTag(tagname);
}


