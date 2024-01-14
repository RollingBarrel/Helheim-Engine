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

void GameObject::Load(const rapidjson::Value& gameObjectJson) {
	GameObject* scene = new GameObject("Scene", nullptr);
	// Manage GameObjects inside the Scene
	if (gameObjectJson.HasMember("GameObjects") && gameObjectJson["GameObjects"].IsArray()) {
		const rapidjson::Value& gameObjects = gameObjectJson["GameObjects"];
		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++) {
			// TODO check if we can improve this key and value search through reflexion-like or somewhat from rapidjson library
			if (gameObjects[i].IsObject()) {
				const rapidjson::Value& gameObject = gameObjects[i];
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
					if (rotationValues.Size() == 3 && rotationValues[0].IsFloat() && rotationValues[1].IsFloat() && rotationValues[2].IsFloat()) {
						x = rotationValues[0].GetFloat();
						y = rotationValues[1].GetFloat();
						z = rotationValues[2].GetFloat();
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

				// If the parentUID of the GameObject is 0, the parent is the Scene.
				// TODO use this constructor as soon as it's available from the GameObject team
				//GameObject go = new GameObject(name, scene, position, scale, rotation);
				GameObject* go;
				
				if (parentUID == 0) {
					go = new GameObject(name, scene);
					go->SetID(uuid);
					go->SetPosition(position);
					go->SetRotation(rotation);
					go->SetScale(scale);

					// Manage Components
					// TODO put this into a function
					if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
						const rapidjson::Value& components = gameObject["Components"];
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

					scene->AddChild(go, parentUID);
				}
				else {
					GameObject* gameObjectParent = findGameObjectParent(scene, parentUID);
					go = new GameObject(name, gameObjectParent);
					go->SetID(uuid);
					go->SetPosition(position);
					go->SetRotation(rotation);
					go->SetScale(scale);

					// Manage Components
					// TODO put this into a function
					if (gameObject.HasMember("Components") && gameObject["Components"].IsArray()) {
						const rapidjson::Value& components = gameObject["Components"];
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

					gameObjectParent->AddChild(go);

				}

				
			}
		}
	}
}


