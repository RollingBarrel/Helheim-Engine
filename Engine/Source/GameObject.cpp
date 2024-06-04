#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"

#include "Component.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
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
#include "BoxColliderComponent.h"
#include "TrailComponent.h"

#include <algorithm>
#include "Algorithm/Random/LCG.h"
#include "MathFunc.h"
#include "Quadtree.h"


#pragma region Constructors and Destructors

GameObject::GameObject(const char* name) : GameObject(LCG().Int(), name , App->GetScene()->GetRoot())
{
}

GameObject::GameObject(GameObject* parent) : GameObject(LCG().Int(), "GameObject", parent)
{
}

GameObject::GameObject(const char* name, GameObject* parent) : GameObject(LCG().Int(), name, parent)
{
}

GameObject::GameObject(unsigned int uid, const char* name, GameObject* parent)
	:mUid(uid), mName(name), mParent(parent), mTag("Untagged"),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot)
	{
		App->GetScene()->AddGameObjectToScene(this);

		mWorldTransformMatrix = mParent->GetWorldTransform();
		mIsActive = parent->mIsActive;
		parent->AddChild(this);
		//AddSuffix();
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mUid(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(newParent->mIsActive&& original.mIsEnabled),
	mWorldTransformMatrix(original.mWorldTransformMatrix), mLocalTransformMatrix(original.mLocalTransformMatrix),
	mTag(original.GetTag()), mPrefabId(original.mPrefabId), mPrefabOverride(original.mPrefabOverride)
{
	for (Component* component : original.mComponents)
	{
		mComponents.push_back(component->Clone(this));
	}

	App->GetScene()->AddGameObjectToScene(this);

	for (GameObject* child : original.mChildren)
	{
		GameObject* gameObject = new GameObject(*(child), this);
		mParent->AddChild(gameObject);
	}
}

GameObject::~GameObject()
{
	for (Component* component : mComponents)
	{
		delete component;
	}
	mComponents.clear();
 
	App->GetScene()->RemoveGameObjectFromScene(this);

	for (GameObject* gameObject : mChildren)
	{
		delete gameObject;
	}
	mChildren.clear();
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

	if (mComponentsToDelete.size() > 0)
	{
		DeleteComponents();
	}
}

#pragma endregion

#pragma region Getter & Setters
AABB GameObject::GetAABB()
{
	std::vector<Component*> components;
	GetComponentsInChildren(ComponentType::MESHRENDERER, components);

	AABB mixedAABB;
	mixedAABB.SetNegativeInfinity();

	for (Component* component : components)
	{
		mixedAABB.Enclose(reinterpret_cast<MeshRendererComponent*>(component)->GetAABB());
	}

	return mixedAABB;
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent && "Parent was nullptr");
	mParent->RemoveChild(mUid);
	mParent = newParent;
	mParent->AddChild(this);

	RecalculateMatrices();
	SetActiveInHierarchy(mParent->mIsActive && mIsEnabled);
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
	math::Quat xQuat = Quat::FromEulerXYZ(rotationInRadians.x, 0, 0);
	math::Quat yQuat = Quat::FromEulerXYZ(0, rotationInRadians.y, 0);
	math::Quat zQuat = Quat::FromEulerXYZ(0, 0, rotationInRadians.z);
	mRotation = yQuat * xQuat * zQuat;

	mEulerAngles = rotationInRadians;
	mIsTransformModified = true;
}

void GameObject::SetLocalRotation(const float3& rotationInRadians)
{
	math::Quat xQuat = Quat::FromEulerXYZ(rotationInRadians.x, 0, 0);
	math::Quat yQuat = Quat::FromEulerXYZ(0, rotationInRadians.y, 0);
	math::Quat zQuat = Quat::FromEulerXYZ(0, 0, rotationInRadians.z);
	mLocalRotation = yQuat * xQuat * zQuat;

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

#pragma endregion

#pragma region Components

template<typename T>
T* GameObject::CreateComponent()
{
	return new T(this);
}

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
	case ComponentType::POINTLIGHT:		//TODO: Redoo PointLights
	{
		const float3 pos = mPosition;
		const PointLight def = { pos.x, pos.y, pos.z, 25.0f, 1.f, 1.f, 1.f, 50.0f };
		newComponent = new PointLightComponent(this, def);
		break;
	}
	case ComponentType::SPOTLIGHT:
		newComponent = new SpotLightComponent(this);
		break;
	case ComponentType::SCRIPT:
		newComponent = new ScriptComponent(this);
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
	case ComponentType::SLIDER:				//TODO: Redoo UI To not create gameObjects in a component	
		newComponent = new SliderComponent(this);
		break;
	case ComponentType::PARTICLESYSTEM:
		newComponent = new ParticleSystemComponent(this);
		break;
	
	default:
		break;
	}

	if (newComponent)
	{
		mComponents.push_back(newComponent);
	}
	if (type == ComponentType::MESHRENDERER)		//TODO: WTF is this doing here?
	{
		App->GetScene()->GetQuadtreeRoot()->AddObject(this);
	}
	return newComponent;
}

template<typename T> 
T* GameObject::GetComponent() const
{
	ComponentType type = T::GetType();
	for (Component* component : mComponents)
	{
		if (component->GetType() == type)
		{
			return component;
		}
	}
	return nullptr;
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

void GameObject::GetComponentsInChildren(ComponentType type, std::vector<Component*>& componentVector) const
{
	Component* gameObjectComponent = GetComponent(type);

	if (gameObjectComponent)
	{
		componentVector.push_back(gameObjectComponent);
	}

	for (GameObject* child : mChildren)
	{
		child->GetComponentsInChildren(type, componentVector);
	}
}

template<typename T>
T* GameObject::GetComponentInParent() const
{
	Component* component = nullptr;
	const GameObject* parent = this;
	while (parent)
	{
		component = parent->GetComponent(T::GetType());

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
	for (std::vector<Component*>::const_iterator it = mComponentsToDelete.cbegin(); it > mComponentsToDelete.cend(); ++it)
	{
		mComponents.erase(it);
		delete* it;
	}
	/*for (auto component : mComponentsToDelete)
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), component);
		if (it != mComponents.end())
		{
			mComponents.erase(it);
			delete component;
			component = nullptr;
		}
	}*/
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
	obj.AddInt("UID", mUid);
	if (mParent == App->GetScene()->GetRoot())
	{
		obj.AddInt("ParentUID", 1);
	}
	else
	{
		obj.AddInt("ParentUID", mParent->GetID());
	}
	obj.AddString("Name", mName.c_str());
	obj.AddBool("Enabled", mIsEnabled);
	obj.AddFloats("Translation", mPosition.ptr(), 3);
	obj.AddFloats("Rotation", mRotation.ptr(), 4);
	obj.AddFloats("Scale", mScale.ptr(), 3);
	obj.AddString("Tag", mTag.c_str());
	obj.AddInt("PrefabUid", mPrefabId);
	obj.AddBool("OverridePrefab", mPrefabOverride);
	obj.AddBool("Dynamic", mIsDynamic);

	// Save components
	JsonArray components = obj.AddNewJsonArray("Components");
	for (const Component* component : mComponents)
	{
		JsonObject comp = components.PushBackNewObject();
		component->Save(comp);
	}
}

void GameObject::Load(const JsonObject& jsonObject)
{
	mIsEnabled = jsonObject.GetBool("Enabled");
	float pos[3]; 
	jsonObject.GetFloats("Translation", pos);
	SetPosition(float3(pos));
	float rot[4]; 
	jsonObject.GetFloats("Rotation", rot);
	SetRotation(Quat(rot));
	float scale[3]; 
	jsonObject.GetFloats("Scale", scale);
	SetScale(float3(scale));

	mTag = jsonObject.GetString("Tag");
	mPrefabId = jsonObject.GetInt("PrefabUid");
	mPrefabOverride = jsonObject.GetBool("OverridePrefab");
	mIsDynamic = jsonObject.GetBool("Dynamic");

	// Load components
	JsonArray components = jsonObject.GetJsonArray("Components");
	for (unsigned int i = 0; i < components.Size(); ++i)
	{
		JsonObject componentData = components.GetJsonObject(i);
		ComponentType cType = (ComponentType) componentData.GetInt("ComponentType");
		Component* component = this->CreateComponent(cType);
		component->Load(componentData);
	}
}

void GameObject::LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id)
{
	if (mPrefabOverride && mPrefabId == id)
	{
		std::vector<GameObject*> loadedObjects;
		for (GameObject* child : mChildren)
		{
			delete child;
		}
		mChildren.clear();
		std::unordered_map<int, int> uuids;
		
		if (gameObject.HasMember("GameObjects") && gameObject["GameObjects"].IsArray())
		{
			const rapidjson::Value& gameObjects = gameObject["GameObjects"];
			for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
			{
				if (gameObjects[i].IsObject())
				{
					unsigned int parentUID{ 0 };
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
							loadedObjects.push_back(this);
							uuids[uuid] = mUid;
							//LoadComponentsFromJSON(gameObjects[i]["Components"]);
						}
					}
					else
					{
						//GameObject* go = LoadGameObjectFromJSON(gameObjects[i], this, &uuids);
						//loadedObjects.push_back(go);
						//go->LoadComponentsFromJSON(gameObjects[i]["Components"]);
					}
				}
			}
			mParent->RecalculateMatrices();
			for (rapidjson::SizeType i = 0; i < gameObjects.Size(); i++)
			{
				if (gameObjects[i].IsObject())
				{
					//loadedObjects[i]->LoadComponentsFromJSON(gameObjects[i]["Components"]);
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

	for (GameObject* child : mChildren)
	{

		if (child->GetName().compare(name) == 0)
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

	for (GameObject* child : mChildren)
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

//GameObject* GameObject::FindGameObjectWithTag(std::string tagname)
//{
//	Tag* tag = App->GetScene()->GetTagByName(tagname);
//
//	if (tag != nullptr)
//	{
//		return App->GetScene()->FindGameObjectWithTag(tag->GetID());
//	}
//	else
//	{
//		return nullptr;
//	}
//
//}
//
//std::vector<GameObject*>& GameObject::FindGameObjectsWithTag(std::string tagname)
//{
//	std::vector<GameObject*> foundGameObjects;
//	Tag* tag = App->GetScene()->GetTagByName(tagname);
//	App->GetScene()->FindGameObjectsWithTag(tag->GetID(), foundGameObjects);
//
//	return foundGameObjects;
//}
#pragma endregion

#pragma region Children

void GameObject::AddChild(GameObject* child)
{
	mChildren.push_back(child);	
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* object = nullptr;
	for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it)
	{
		if ((*it)->GetID() == id)
		{
			object = *it;
			mChildren.erase(it);
			break;
		}
	}
	
	return object;
}

#pragma endregion
