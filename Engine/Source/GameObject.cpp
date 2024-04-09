#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"

#include "Component.h"
#include <algorithm>
#include <unordered_map>
#include "MathFunc.h"

#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "TestComponent.h"
#include "AIAgentComponent.h"
#include "NavMeshObstacleComponent.h"
#include "AnimationComponent.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ButtonComponent.h"
#include "ScriptComponent.h"

#include "Tag.h"
#include "Quadtree.h"
#include <regex>

GameObject::GameObject(GameObject* parent)
	:mID(LCG().Int()), mName("GameObject"), mParent(parent), mTag(App->GetScene()->GetTagByName("Untagged")),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
		AddSuffix();
	}
	

}

GameObject::GameObject(const GameObject& original)
	:mID(LCG().Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(original.mIsActive), 
	mWorldTransformMatrix(original.mWorldTransformMatrix), mLocalTransformMatrix(original.mLocalTransformMatrix), mTag(original.GetTag()),
	mPrefabResourceId(original.mPrefabResourceId), mPrefabOverride(original.mPrefabOverride)
{

	if (mParent) {
		AddSuffix();
	}

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
	mTag(original.GetTag()), mPrefabResourceId(original.mPrefabResourceId), mPrefabOverride(original.mPrefabOverride)
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
	:mID(LCG().Int()), mName(name), mParent(parent), mTag(App->GetScene()->GetTagByName("Untagged")),
	mIsRoot(parent == nullptr)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
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


Component* GameObject::GetComponent(ComponentType type) const
{
	for (auto component : mComponents) {
		if (component->GetType() == type) {
			return component;
		}
	}
	return nullptr;
}

std::vector<Component*> GameObject::GetComponents(ComponentType type) const
{
	std::vector<Component*> matchingComponents;

	for (auto component : mComponents) {
		if (component->GetType() == type) {
			matchingComponents.push_back(component);
		}
	}

	return matchingComponents;
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

	if (GetComponent(ComponentType::CAMERA) != nullptr) {
		CameraComponent* camera = (CameraComponent*)GetComponent(ComponentType::CAMERA);
		camera->Reset();
	}
}

void GameObject::SetEnabled(bool enabled)
{
	mIsEnabled = enabled;

	if (!enabled || IsRoot() || mParent->IsActive())
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
	//float3 difference = rotationInRadians - mEulerRotation;
	//Quat deltaRotation = Quat::FromEulerXYZ(rotationInRadians.x - mEulerRotation.x , rotationInRadians.y - mEulerRotation.y, rotationInRadians.z - mEulerRotation.z);
	//mRotation = mRotation * deltaRotation;
	mRotation = Quat::FromEulerXYZ(rotationInRadians.x, rotationInRadians.y, rotationInRadians.z);
	mEulerRotation = rotationInRadians;

	//if (GetComponent(ComponentType::CAMERA) != nullptr) {
		//CameraComponent* camera = (CameraComponent*)GetComponent(ComponentType::CAMERA);
		//camera->SetRotation(difference);
	//}

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

	if (GetComponent(ComponentType::CAMERA) != nullptr) {
		CameraComponent* camera = (CameraComponent*)GetComponent(ComponentType::CAMERA);
		camera->SetPosition(difference);
	}
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;

	isTransformModified = true;
}

GameObject* GameObject::Find(const char* name) const
{

	GameObject* gameObject = nullptr;

	for (auto child : mChildren) {

		if (child->GetName()._Equal(std::string(name))) {
			gameObject = child;
			break;
		}
		else {
			gameObject = child->Find(name);

			if (gameObject) {
				break;
			}
		}

	}

	return gameObject;
}

GameObject* GameObject::Find(unsigned int UID) const
{
	GameObject* gameObject = nullptr;

	for (auto child : mChildren) {

		if (child->GetID() == UID) {
			gameObject = child;
			break;
		}
		else {
			gameObject = child->Find(UID);

			if (gameObject) {
				break;
			}
		}

	}

	return gameObject;
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
	bool hasNextItemSufix = false;
	//size_t lastPos = -1;
	while (found) {
		std::regex regularExpression(".+\\s\\(\\d+\\)$");

		std::string sufix = " (" + std::to_string(count) + ')';
		size_t pos = std::string::npos;
		size_t hasSufix = std::string::npos;
		std::string nameWithoutSufix = mName;

		if (std::regex_match(mName, regularExpression) || hasNextItemSufix)
		{
			hasSufix = mName.rfind(" (");

			if (hasSufix != std::string::npos) {
				nameWithoutSufix.erase(hasSufix);
			}

			std::string nameWithSufix = nameWithoutSufix + sufix;

			for (auto gameObject : mParent->mChildren)
			{
				if (pos == std::string::npos) {
					pos = gameObject->mName.find(nameWithSufix);
				}

			}

			if (pos == std::string::npos) {
				if (mParent->mChildren.size() > 0) {
					mName = nameWithSufix;
				}
				found = false;
			}
			else {
				count++;
				//lastPos = pos;
			}
		}
		else 
		{
			for (auto child : mParent->mChildren)
			{
				if (pos == std::string::npos && child != this) {
					pos = child->mName.find(mName);
				}

			}

			size_t isObjectWithSufix = std::string::npos;
			for (auto child : mParent->mChildren) {
				if (isObjectWithSufix == std::string::npos && child != this) {
					isObjectWithSufix = child->mName.find(mName + sufix);
				}
			}

			if (pos != std::string::npos && isObjectWithSufix == std::string::npos) {
				mName += sufix;
				found = false;
			}
			else if (isObjectWithSufix != std::string::npos){
				hasNextItemSufix = true;
			}
			else {
				found = false;
			}
			
			
		}
	}
}

//TODO: Crate a component that requires ids not clean now
Component* GameObject::CreateComponent(ComponentType type) {

	Component* newComponent = nullptr;

	switch (type) {
		case ComponentType::MESHRENDERER:
			newComponent = new MeshRendererComponent(this);
			break;
		case ComponentType::CAMERA:
			newComponent = new CameraComponent(this);
			break;
		case ComponentType::POINTLIGHT:
		{
			const float3& pos = GetWorldPosition();
			newComponent = App->GetOpenGL()->AddPointLight({ pos.x, pos.y, pos.z, 25.0f, 1.f, 1.f, 1.f, 50.0f }, this);
			break;
		}
		case ComponentType::SPOTLIGHT:
		{
			const float3& pos = GetWorldPosition();
			newComponent = App->GetOpenGL()->AddSpotLight({ 25.f , 0.0f, 0.0f, 0.0f, pos.x, pos.y, pos.z, 50.0f, 0.f, -1.f, 0.f, cos(DegToRad(25.f)), 1.f, 1.f, 1.f , cos(DegToRad(38.f)) }, this);
			break;
		}
		case ComponentType::SCRIPT:
			newComponent = new ScriptComponent(this);
			break;
		case ComponentType::TEST:
			newComponent = new TestComponent(this);
			break;
		case ComponentType::AIAGENT:
			newComponent = new AIAgentComponent(this);
			break;
		case ComponentType::NAVMESHOBSTACLE:
			newComponent = new NavMeshObstacleComponent(this);
			break;
		case ComponentType::ANIMATION:
			newComponent = new AnimationComponent(this);
			break;
		case ComponentType::IMAGE:
			newComponent = new ImageComponent(this);
			break;
		case ComponentType::CANVAS:
			newComponent = new CanvasComponent(this);
			break;
		case ComponentType::BUTTON:
			newComponent = new ButtonComponent(this);
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
	if (GetComponent(ComponentType::MESHRENDERER) != nullptr)
	{
		((MeshRendererComponent*)GetComponent(ComponentType::MESHRENDERER))->RefreshBoundingBoxes();
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

	for (Component* component : mComponents)
	{
		if (active)
		{
			component->Enable();
		}
		else
		{
			component->Disable();
		}
	}


	for (GameObject* child : mChildren)
	{
		child->SetActiveInHierarchy(active);
	}



}

void GameObject::Save(Archive& archive, int parentId) const {
	archive.AddInt("UID", mID);
	if (mParent->GetID() == parentId) {
		archive.AddInt("ParentUID", 1);
	}
	else { archive.AddInt("ParentUID", mParent->GetID()); }
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

static GameObject* FindGameObjectParent(GameObject* gameObject, int UID) {
	GameObject* gameObjectParent = nullptr;
	const std::vector<GameObject*>& gameObjects = gameObject->GetChildren();
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetID() == UID) {
			// Found the parent
			gameObjectParent = gameObjects[i];
			break;
		}
		else {
			// Recursively search in children
			GameObject* gameObjectChild = FindGameObjectParent(gameObjects[i], UID);
			if (gameObjectChild != nullptr) {
				// Found a match in children, return it as the gameobject parent
				gameObjectParent = gameObjectChild;
				break;
			}
		}
	}

	return gameObjectParent;
}

static void LoadComponentsFromJSON(const rapidjson::Value& components, GameObject* go) {
	for (rapidjson::SizeType i = 0; i < components.Size(); i++) {
		if (components[i].IsObject()) {
			const rapidjson::Value& componentValue = components[i];
			if (componentValue.HasMember("ComponentType") && componentValue["ComponentType"].IsInt()) {
				ComponentType cType = ComponentType(componentValue["ComponentType"].GetInt());
				Component* component = go->GetComponent(cType);
				if (!component) { 
					component = go->CreateComponent(cType); 
				}
				component->LoadFromJSON(componentValue, go);
			}
		}
	}
}
void GameObject::LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id) {
	if (mPrefabOverride && mPrefabResourceId == id) {
		for (GameObject* child : mChildren) {
			RemoveChild(child->GetID());
		}
		std::unordered_map<int, int> uuids;
		if (gameObject.HasMember("GameObjects") && gameObject["GameObjects"].IsArray()) {
			const rapidjson::Value& gameObjects = gameObject["GameObjects"];
			for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++) {
				if (gameObjects[i].IsObject()) {
					int parentUID{ 0 };
					unsigned int uuid{ 0 };

					if (gameObject.HasMember("ParentUID") && gameObject["ParentUID"].IsInt()) {
						parentUID = gameObject["ParentUID"].GetInt();
					}
					if (gameObject.HasMember("UID") && gameObject["UID"].IsInt()) {
						uuid = gameObject["UID"].GetInt();
					}
					if (parentUID != 1) {
						uuids[uuid] = mID;
					}
					else {
						LoadGameObjectFromJSON(gameObjects[i], this, &uuids);
					}
				}
			}
		}
	}
	else {
		for (GameObject* child : mChildren) {
			child->LoadChangesPrefab(gameObject, id);
		}
	}
	
}

void LoadGameObjectFromJSON(const rapidjson::Value& gameObject, GameObject* scene, std::unordered_map<int, int>* convertUuid) {
	unsigned int uuid{ 0 };
	int parentUID{ 0 };
	const char* name = { "" };
	float3 position;
	float3 scale;
	Quat rotation;
	Tag* tag = App->GetScene()->GetTagByName("Untagged");

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
		int tagid = tagint.GetInt();
		Tag* loadedTag = App->GetScene()->GetTagByID(tagid);

		if (loadedTag != nullptr) {
			tag = loadedTag;
		}
	}

	GameObject* go;

	if (parentUID == 1) {
		go = new GameObject(name, scene);
	}
	else {
		GameObject* gameObjectParent = FindGameObjectParent(scene, (*convertUuid)[parentUID]);
		go = new GameObject(name, gameObjectParent);
	}
	go->SetPosition(position);
	go->SetRotation(rotation);
	go->SetScale(scale);
	// Manage Components
	if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
		LoadComponentsFromJSON(gameObject["Components"], go);
	}
	(*convertUuid)[uuid] = go->GetID();
	go->SetTag(tag);
}

void GameObject::Load(const rapidjson::Value& gameObjectsJson) {
	GameObject* scene = App->GetScene()->GetRoot();
	std::unordered_map<int, int> uuids;
	// Manage GameObjects inside the Scene
	if (gameObjectsJson.HasMember("GameObjects") && gameObjectsJson["GameObjects"].IsArray()) {
		const rapidjson::Value& gameObjects = gameObjectsJson["GameObjects"];
		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++) {
			if (gameObjects[i].IsObject()) {
				LoadGameObjectFromJSON(gameObjects[i], this, &uuids);
			}
		}
	}
}

GameObject* GameObject::FindGameObjectWithTag(std::string tagname)
{
	Tag* tag = App->GetScene()->GetTagByName(tagname);

	if (tag != nullptr) {
		return App->GetScene()->FindGameObjectWithTag(App->GetScene()->GetRoot(), tag->GetID());
	}
	else {
		return nullptr;
	}

}

std::vector<GameObject*> GameObject::FindGameObjectsWithTag(std::string tagname)
{
	std::vector<GameObject*> foundGameObjects;
	Tag* tag = App->GetScene()->GetTagByName(tagname);
	App->GetScene()->FindGameObjectsWithTag(App->GetScene()->GetRoot(), tag->GetID(), foundGameObjects);

	return foundGameObjects;
}
