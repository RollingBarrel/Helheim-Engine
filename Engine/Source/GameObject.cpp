#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"

#include "Component.h"
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
#include "AudioSourceComponent.h"
#include "AudioListenerComponent.h"
#include "Transform2DComponent.h"
#include "SliderComponent.h"
#include "ParticleSystemComponent.h"

#include <algorithm>
#include "Algorithm/Random/LCG.h"
#include <unordered_map>
#include "MathFunc.h"
#include "Tag.h"
#include "Quadtree.h"
#include <regex>

#pragma region Constructors and Destructors

GameObject::GameObject(GameObject* parent) : GameObject(LCG().Int(), "GameObject", parent)
{
}

GameObject::GameObject(const char* name, GameObject* parent) : GameObject(LCG().Int(), name, parent)
{
}

GameObject::GameObject(unsigned int ID, const char* name, GameObject* parent)
	:mID(ID), mName(name), mParent(parent), mTag(App->GetScene()->GetTagByName("Untagged")),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot)
	{
		App->GetScene()->AddGameObjectToScene(this);

		mWorldTransformMatrix = mParent->GetWorldTransform();
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
		AddSuffix();
	}
}

GameObject::GameObject(const GameObject& original) : GameObject(original, original.mParent)
{
	if (mParent)
	{
		AddSuffix();
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(newParent->mIsActive&& original.mIsEnabled),
	mWorldTransformMatrix(original.mWorldTransformMatrix), mLocalTransformMatrix(original.mLocalTransformMatrix),
	mTag(original.GetTag()), mPrefabResourceId(original.mPrefabResourceId), mPrefabOverride(original.mPrefabOverride)
{
	App->GetScene()->AddGameObjectToScene(this);

	for (auto child : original.mChildren)
	{
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents)
	{
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::~GameObject()
{
	App->GetScene()->RemoveGameObjectFromScene(this);

	for (GameObject* gameObject : mChildren)
	{
		delete gameObject;
		gameObject = nullptr;
	}
	mChildren.clear();

	for (Component* component : mComponents)
	{
		delete component;
		component = nullptr;
	}
	mComponents.clear();
}

#pragma endregion

#pragma region Base functions

void GameObject::Update()
{
	if (IsActive())
	{
		if (mIsTransformModified)
		{
			RecalculateMatrices();
			RefreshBoundingBoxes();
		}
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update();
		}

		for (size_t i = 0; i < mChildren.size(); i++)
		{
			mChildren[i]->Update();
		}
		mIsTransformModified = false;
	}

	DeleteComponents();
}

#pragma endregion

#pragma region Getter & Setters
AABB GameObject::GetAABB()
{
	std::vector<Component*> components = GetComponentsInChildren(ComponentType::MESHRENDERER);

	AABB mixedAABB;
	mixedAABB.SetNegativeInfinity();

	for (Component* component : components)
	{
		mixedAABB.Enclose(reinterpret_cast<MeshRendererComponent*>(component)->GetAABB());
	}

	return mixedAABB;
}

void GameObject::SetEnabled(bool enabled)
{
	// TODO, kind of redundant, no?
	if (mIsEnabled != enabled)
	{
		mIsEnabled = enabled;

		if (!enabled || IsRoot() || mParent->IsActive())
		{
			SetActiveInHierarchy(enabled);
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
		component->SetEnable(active);
	}


	for (GameObject* child : mChildren)
	{
		child->SetActiveInHierarchy(active);
	}
}

#pragma endregion

#pragma region Transform
void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);

	mWorldTransformMatrix = mLocalTransformMatrix;
	if (mParent != nullptr)
	{
		mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;
	}

	for (size_t i = 0; i < mChildren.size(); i++)
	{
		mChildren[i]->RecalculateMatrices();
	}

}

//Position
void GameObject::SetPosition(const float3& position)
{
	mPosition = position;
	mIsTransformModified = true;
}

void GameObject::SetLocalPosition(const float3& position)
{
	mLocalPosition = position;
	mIsTransformModified = true;
}

void GameObject::SetRotation(const float3& rotationInRadians)
{
	mRotation = Quat::FromEulerXYZ(rotationInRadians.x, rotationInRadians.y, rotationInRadians.z);
	mEulerAngles = rotationInRadians;
	mIsTransformModified = true;
}

void GameObject::SetLocalRotation(const float3& rotationInRadians)
{
	mLocalRotation = Quat::FromEulerXYZ(rotationInRadians.x, rotationInRadians.y, rotationInRadians.z);
	mLocalEulerAngles = rotationInRadians;
	mIsTransformModified = true;
}

void GameObject::SetRotation(const Quat& rotation)
{
	mRotation = rotation;
	mEulerAngles = rotation.ToEulerXYZ();
	mIsTransformModified = true;
}

void GameObject::SetLocalRotation(const Quat& rotation)
{
	mLocalRotation = rotation;
	mLocalEulerAngles = rotation.ToEulerXYZ();
	mIsTransformModified = true;
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;
	mIsTransformModified = true;
}

void GameObject::SetLocalScale(const float3& scale)
{
	mLocalScale = scale;
	mIsTransformModified = true;
}

const bool GameObject::HasUpdatedTransform() const
{
	if (!mIsTransformModified && mParent != nullptr)
	{
		if (mParent->HasUpdatedTransform())
		{
			return true;
		}
	}
	return mIsTransformModified;
}

void GameObject::Translate(float3 translation)
{
	SetPosition(mPosition + translation);
}

void GameObject::LookAt(float3 target)
{
	float4x4 rotationMatrix = float4x4::identity;

	float3 forward = -(target - GetPosition()).Normalized();
	float3 right = Cross(forward, float3::unitY).Normalized();
	float3 up = Cross(right, forward).Normalized();

	rotationMatrix[0][0] = right.x;
	rotationMatrix[1][0] = right.y;
	rotationMatrix[2][0] = right.z;
	rotationMatrix[0][1] = up.x;
	rotationMatrix[1][1] = up.y;
	rotationMatrix[2][1] = up.z;
	rotationMatrix[0][2] = -forward.x;
	rotationMatrix[1][2] = -forward.y;
	rotationMatrix[2][2] = -forward.z;


	mRotation = Quat(rotationMatrix);
	mEulerAngles = mRotation.ToEulerXYZ();

	mIsTransformModified = true;
}

void GameObject::ResetTransform()
{
	SetPosition(float3::zero);
	SetRotation(float3::zero);
	SetScale(float3::one);
}

void GameObject::RefreshBoundingBoxes()
{
	if (GetComponent(ComponentType::MESHRENDERER) != nullptr)
	{
		((MeshRendererComponent*)GetComponent(ComponentType::MESHRENDERER))->RefreshBoundingBoxes();
		App->GetScene()->SetShouldUpdateQuadtree(true);

	}
	else
	{
		for (auto children : mChildren)
		{
			children->RefreshBoundingBoxes();
		}
	}
}
#pragma endregion

#pragma region Components
//TODO: Crate a component that requires ids not clean now
Component* GameObject::CreateComponent(ComponentType type)
{

	Component* newComponent = nullptr;

	switch (type)
	{
	case ComponentType::MESHRENDERER:
		newComponent = new MeshRendererComponent(this);
		break;
	case ComponentType::CAMERA:
		newComponent = new CameraComponent(this);
		break;
	case ComponentType::POINTLIGHT:
	{
		const float3 pos = mPosition;
		const PointLight def = { pos.x, pos.y, pos.z, 25.0f, 1.f, 1.f, 1.f, 50.0f };
		newComponent = new PointLightComponent(this, def);
		break;
	}
	case ComponentType::SPOTLIGHT:
	{
		const float3 pos = mPosition;
		const SpotLight def = { 25.f , 0.0f, 0.0f, 0.0f, pos.x, pos.y, pos.z, 50.0f, 0.f, -1.f, 0.f, cos(DegToRad(25.f)), 1.f, 1.f, 1.f , cos(DegToRad(38.f)) };
		newComponent = new SpotLightComponent(this, def);
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
	case ComponentType::TRANSFORM2D:
		newComponent = new Transform2DComponent(this);
		break;
	case ComponentType::AUDIOSOURCE:
		newComponent = new AudioSourceComponent(this);
		break;
	case ComponentType::AUDIOLISTENER:
		newComponent = new AudioListenerComponent(this);
		break;
	case ComponentType::SLIDER:
		newComponent = new SliderComponent(this);
		break;
	case ComponentType::PARTICLESYSTEM:
	{
		ParticleSystemComponent* pc = new ParticleSystemComponent(this);
		newComponent = pc;
		break;
	}
	default:
		break;
	}

	if (newComponent)
	{
		mComponents.push_back(newComponent);
	}
	if (type == ComponentType::MESHRENDERER)
	{
		App->GetScene()->GetQuadtreeRoot()->AddObject(this);
	}
	return newComponent;
}

Component* GameObject::GetComponent(ComponentType type) const
{
	for (Component* component : mComponents)
	{
		if (component->GetType() == type)
		{
			return component;
		}
	}
	return nullptr;
}

std::vector<Component*>& GameObject::GetComponents(ComponentType type) const
{
	std::vector<Component*> matchingComponents;

	for (auto component : mComponents)
	{
		if (component->GetType() == type)
		{
			matchingComponents.push_back(component);
		}
	}

	return matchingComponents;
}

std::vector<Component*>& GameObject::GetComponentsInChildren(ComponentType type) const
{
	std::vector<Component*> componentVector;

	Component* gameObjectComponent = GetComponent(type);

	if (gameObjectComponent)
	{
		componentVector.push_back(gameObjectComponent);
	}

	for (GameObject* child : mChildren)
	{
		std::vector<Component*> childComponents = child->GetComponentsInChildren(type);
		if (!childComponents.empty())
		{
			componentVector.insert(componentVector.end(), childComponents.begin(), childComponents.end());
		}

	}

	return componentVector;
}

Component* GameObject::GetComponentInParent(ComponentType type) const
{
	Component* component = nullptr;
	const GameObject* parent = this;
	while (parent)
	{
		component = parent->GetComponent(type);

		if (component)
		{
			return component;
		}
		else
		{
			parent = parent->mParent;
		}
	}
	return nullptr;
}

void GameObject::AddComponent(Component* component, Component* position)
{
	if (position == nullptr)
	{
		mComponents.push_back(component);
	}
	else
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), position);
		mComponents.insert(it, component);
	}
}

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}

void GameObject::DeleteComponents()
{
	for (auto component : mComponentsToDelete)
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), component);
		if (it != mComponents.end())
		{
			mComponents.erase(it);
			delete component;
			component = nullptr;
		}
	}
}

Component* GameObject::RemoveComponent(Component* component)
{
	Component* movedComponent = nullptr;
	for (auto it = mComponents.begin(); it != mComponents.cend(); ++it)
	{
		if ((*it)->GetID() == component->GetID())
		{
			movedComponent = *it;
			mComponents.erase(it);
			break;
		}
	}
	return movedComponent;
}
#pragma endregion

#pragma region Save / Load

void GameObject::Save(JsonObject& obj) const
{
	obj.AddInt("UID", mID);
	if (mParent == App->GetScene()->GetRoot())
	{
		obj.AddInt("ParentUID", 1);
	}
	else
	{
		obj.AddInt("ParentUID", mParent->GetID());
	}
	obj.AddString("Name", mName.c_str());
	obj.AddBool("isEnabled", mIsEnabled);
	obj.AddFloats("Translation", mPosition.ptr(), 3);
	obj.AddFloats("Rotation", mRotation.ptr(), 4);
	obj.AddFloats("Scale", mScale.ptr(), 3);
	obj.AddInt("Tag", mTag->GetID());
	obj.AddInt("PrefabId", mPrefabResourceId);
	obj.AddBool("OverridePrefab", mPrefabOverride);
	obj.AddBool("Dynamic", mIsDynamic);
	JsonArray components = obj.AddNewJsonArray("Components");
	// Save components
	for (const Component* component : mComponents)
	{
		JsonObject comp = components.PushBackNewObject();
		component->Save(comp);
	}
}

void GameObject::Load(const rapidjson::Value& gameObjectsJson)
{
	// Manage GameObjects inside the Scene
	if (gameObjectsJson.HasMember("GameObjects") && gameObjectsJson["GameObjects"].IsArray())
	{
		const rapidjson::Value& gameObjects = gameObjectsJson["GameObjects"];
		for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
		{
			if (gameObjects[i].IsObject())
			{
				//LoadGameObjectFromJSON(gameObjects[i], this);
			}
		}
	}
	RecalculateMatrices();
}

void GameObject::LoadComponentsFromJSON(const rapidjson::Value& components)
{
	for (rapidjson::SizeType i = 0; i < components.Size(); i++)
	{
		if (components[i].IsObject())
		{
			const rapidjson::Value& componentValue = components[i];
			if (componentValue.HasMember("ComponentType") && componentValue["ComponentType"].IsInt())
			{
				ComponentType cType = ComponentType(componentValue["ComponentType"].GetInt());
				Component* component = this->CreateComponent(cType);
				component->LoadFromJSON(componentValue, this);
			}
		}
	}
}

GameObject* GameObject::LoadGameObjectFromJSON(const rapidjson::Value& gameObject, GameObject* parent)
{
	unsigned int uuid{ 0 };
	int parentUID{ 0 };
	int prefabId{ 0 };
	bool overridePrefab = true;
	const char* name = { "" };
	float3 position;
	float3 scale;
	Quat rotation;
	bool isEnabled = true;
	bool isDynamic = false;
	Tag* tag = App->GetScene()->GetTagByName("Untagged");

	if (gameObject.HasMember("UID") && gameObject["UID"].IsInt())
	{
		uuid = gameObject["UID"].GetInt();
	}
	if (gameObject.HasMember("ParentUID") && gameObject["ParentUID"].IsInt())
	{
		parentUID = gameObject["ParentUID"].GetInt();
	}
	if (gameObject.HasMember("Name") && gameObject["Name"].IsString())
	{
		name = gameObject["Name"].GetString();
	}
	if (gameObject.HasMember("isEnabled") && gameObject["isEnabled"].IsBool())
	{
		isEnabled = gameObject["isEnabled"].GetBool();
	}
	if (gameObject.HasMember("PrefabId") && gameObject["PrefabId"].IsInt())
	{
		prefabId = gameObject["PrefabId"].GetInt();
	}
	if (gameObject.HasMember("OverridePrefab") && gameObject["OverridePrefab"].IsBool())
	{
		overridePrefab = gameObject["OverridePrefab"].GetBool();
	}
	if (gameObject.HasMember("Translation") && gameObject["Translation"].IsArray())
	{
		const rapidjson::Value& translationValues = gameObject["Translation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (translationValues.Size() == 3 && translationValues[0].IsFloat() && translationValues[1].IsFloat() && translationValues[2].IsFloat())
		{
			x = translationValues[0].GetFloat();
			y = translationValues[1].GetFloat();
			z = translationValues[2].GetFloat();
		}

		position = float3(x, y, z);
	}
	if (gameObject.HasMember("Rotation") && gameObject["Rotation"].IsArray())
	{
		const rapidjson::Value& rotationValues = gameObject["Rotation"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
		if (rotationValues.Size() == 4 && rotationValues[0].IsFloat() && rotationValues[1].IsFloat() && rotationValues[2].IsFloat() && rotationValues[3].IsFloat())
		{
			x = rotationValues[0].GetFloat();
			y = rotationValues[1].GetFloat();
			z = rotationValues[2].GetFloat();
			w = rotationValues[3].GetFloat();
		}

		rotation = Quat(x, y, z, w);
	}

	if (gameObject.HasMember("Scale") && gameObject["Scale"].IsArray())
	{
		const rapidjson::Value& scaleValues = gameObject["Scale"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (scaleValues.Size() == 3 && scaleValues[0].IsFloat() && scaleValues[1].IsFloat() && scaleValues[2].IsFloat())
		{
			x = scaleValues[0].GetFloat();
			y = scaleValues[1].GetFloat();
			z = scaleValues[2].GetFloat();
		}

		scale = float3(x, y, z);
	}

	if (gameObject.HasMember("Tag"))
	{
		const rapidjson::Value& tagint = gameObject["Tag"];
		int tagid = tagint.GetInt();
		Tag* loadedTag = App->GetScene()->GetTagByID(tagid);

		if (loadedTag != nullptr)
		{
			tag = loadedTag;
		}
	}
	if (gameObject.HasMember("Dynamic") && gameObject["Dynamic"].IsBool())
	{
		isDynamic = gameObject["Dynamic"].GetBool();
	}

	GameObject* go;

	if (parentUID == 1)
	{
		go = new GameObject(uuid, name, parent);
	}
	else
	{
		GameObject* gameObjectParent = App->GetScene()->Find(parentUID);
		go = new GameObject(uuid, name, gameObjectParent);
	}

	go->SetPosition(position);
	go->SetRotation(rotation);
	go->SetScale(scale);
	go->SetPrefabId(prefabId);
	go->SetPrefabOverride(overridePrefab);
	go->SetTag(tag);
	go->SetEnabled(isEnabled);
	go->SetDynamic(isDynamic);

	return go;
}

void GameObject::LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id)
{
	if (mPrefabOverride && mPrefabResourceId == id)
	{
		for (GameObject* child : mChildren)
		{
			DeleteChild(child);
		}

		if (gameObject.HasMember("GameObjects") && gameObject["GameObjects"].IsArray())
		{
			const rapidjson::Value& gameObjects = gameObject["GameObjects"];
			for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
			{
				if (gameObjects[i].IsObject())
				{
					int parentUID{ 0 };
					unsigned int uuid{ 0 };

					if (gameObjects[i].HasMember("ParentUID") && gameObjects[i]["ParentUID"].IsInt())
					{
						parentUID = gameObjects[i]["ParentUID"].GetInt();
					}
					if (gameObjects[i].HasMember("UID") && gameObjects[i]["UID"].IsInt())
					{
						uuid = gameObjects[i]["UID"].GetInt();
					}
					if (parentUID == 1) {
						if (gameObjects[i].HasMember("Components") && gameObjects[i]["Components"].IsArray())
						{
							LoadComponentsFromJSON(gameObjects[i]["Components"]);
						}
					}
					else
					{
						LoadGameObjectFromJSON(gameObjects[i], mParent);
					}
				}
			}
		}
	}
	else
	{
		for (GameObject* child : mChildren)
		{
			child->LoadChangesPrefab(gameObject, id);
		}
	}

}
#pragma endregion

#pragma region Finds

GameObject* GameObject::Find(const char* name) const
{

	GameObject* gameObject = nullptr;

	for (auto child : mChildren)
	{

		if (child->GetName()._Equal(std::string(name)))
		{
			gameObject = child;
			break;
		}
		else
		{
			gameObject = child->Find(name);

			if (gameObject)
			{
				break;
			}
		}

	}

	return gameObject;
}

GameObject* GameObject::Find(unsigned int UID) const
{
	GameObject* gameObject = nullptr;

	for (auto child : mChildren)
	{

		if (child->GetID() == UID)
		{
			gameObject = child;
			break;
		}
		else
		{
			gameObject = child->Find(UID);

			if (gameObject)
			{
				break;
			}
		}

	}

	return gameObject;
}

static GameObject* FindGameObjectParent(GameObject* gameObject, int UID)
{
	GameObject* gameObjectParent = nullptr;
	const std::vector<GameObject*>& gameObjects = gameObject->GetChildren();
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->GetID() == UID)
		{
			// Found the parent
			gameObjectParent = gameObjects[i];
			break;
		}
		else
		{
			// Recursively search in children
			GameObject* gameObjectChild = FindGameObjectParent(gameObjects[i], UID);
			if (gameObjectChild != nullptr)
			{
				// Found a match in children, return it as the gameobject parent
				gameObjectParent = gameObjectChild;
				break;
			}
		}
	}
	return gameObjectParent;
}

std::vector<Component*>& GameObject::FindComponentsInChildren(GameObject* parent, const ComponentType type)
{
	std::vector<Component*> components = parent->GetComponents(type);

	std::vector<GameObject*> children = parent->GetChildren();
	for (GameObject* child : children)
	{
		std::vector<Component*> childComponents = FindComponentsInChildren(child, type);
		components.insert(components.end(), childComponents.begin(), childComponents.end());
	}

	return components;
}

GameObject* GameObject::FindGameObjectWithTag(std::string tagname)
{
	Tag* tag = App->GetScene()->GetTagByName(tagname);

	if (tag != nullptr)
	{
		return App->GetScene()->FindGameObjectWithTag(tag->GetID());
	}
	else
	{
		return nullptr;
	}

}

std::vector<GameObject*>& GameObject::FindGameObjectsWithTag(std::string tagname)
{
	std::vector<GameObject*> foundGameObjects;
	Tag* tag = App->GetScene()->GetTagByName(tagname);
	App->GetScene()->FindGameObjectsWithTag(tag->GetID(), foundGameObjects);

	return foundGameObjects;
}
#pragma endregion

#pragma region Children

void GameObject::DeleteChild(GameObject* child)
{
	RemoveChild(child->mID);
	delete child;
	child = nullptr;
}

void GameObject::AddChild(GameObject* child, const int aboveThisId)
{
	child->mParent = this;
	bool inserted = false;
	if (aboveThisId != 0)
	{
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
		{
			if ((*it)->GetID() == aboveThisId)
			{
				mChildren.insert(it, child);
				inserted = true;
				break;
			}
		}
	}

	child->RecalculateMatrices();
	child->SetActiveInHierarchy(mIsActive && child->mIsEnabled);

	if (!inserted)
	{
		mChildren.push_back(child);
	}
	if (child->GetComponent(ComponentType::MESHRENDERER) != nullptr)
	{
		App->GetScene()->GetQuadtreeRoot()->AddObject(child);
	}
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it)
	{
		if ((*it)->GetID() == id)
		{
			if ((*it)->GetComponent(ComponentType::MESHRENDERER) != nullptr)
			{
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
	while (found)
	{
		std::regex regularExpression(".+\\s\\(\\d+\\)$");

		std::string sufix = " (" + std::to_string(count) + ')';
		size_t pos = std::string::npos;
		size_t hasSufix = std::string::npos;
		std::string nameWithoutSufix = mName;

		if (std::regex_match(mName, regularExpression) || hasNextItemSufix)
		{
			hasSufix = mName.rfind(" (");

			if (hasSufix != std::string::npos)
			{
				nameWithoutSufix.erase(hasSufix);
			}

			std::string nameWithSufix = nameWithoutSufix + sufix;

			for (auto gameObject : mParent->mChildren)
			{
				if (pos == std::string::npos)
				{
					pos = gameObject->mName.find(nameWithSufix);
				}

			}

			if (pos == std::string::npos)
			{
				if (mParent->mChildren.size() > 0)
				{
					mName = nameWithSufix;
				}
				found = false;
			}
			else
			{
				count++;
				//lastPos = pos;
			}
		}
		else
		{
			for (auto child : mParent->mChildren)
			{
				if (pos == std::string::npos && child != this)
				{
					pos = child->mName.find(mName);
				}

			}

			size_t isObjectWithSufix = std::string::npos;
			for (auto child : mParent->mChildren)
			{
				if (isObjectWithSufix == std::string::npos && child != this)
				{
					isObjectWithSufix = child->mName.find(mName + sufix);
				}
			}

			if (pos != std::string::npos && isObjectWithSufix == std::string::npos)
			{
				mName += sufix;
				found = false;
			}
			else if (isObjectWithSufix != std::string::npos)
			{
				hasNextItemSufix = true;
			}
			else
			{
				found = false;
			}


		}
	}
}

#pragma endregion
