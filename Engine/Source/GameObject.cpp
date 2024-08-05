#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"
#include "ModuleScriptManager.h"
#include "ModuleCamera.h"

#include "Component.h"
#include "MeshRendererComponent.h"
#include "CameraComponent.h"
#include "AIAgentComponent.h"
#include "NavMeshObstacleComponent.h"
#include "AnimationComponent.h"
#include "ImageComponent.h"
#include "MaskComponent.h"
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
#include "DecalComponent.h"
#include "TextComponent.h"

#include <algorithm>
#include "Algorithm/Random/LCG.h"
#include "MathFunc.h"


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
	:mUid(uid), mName(name), mParent(parent), mIsRoot(parent == nullptr)
{
	if (!mIsRoot)
	{
		SetTag("Untagged");

		App->GetScene()->AddGameObjectToScene(this);

		mIsActive = parent->mIsActive;
		parent->AddChild(this);
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent, std::unordered_map<const GameObject*, GameObject*>* originalToNew, std::vector<MeshRendererComponent*>* meshRendererComps)
	:mUid(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mIsActive(newParent->mIsActive&& original.mIsEnabled),
	mWorldTransformMatrix(original.GetWorldTransform()), mLocalTransformMatrix(original.mLocalTransformMatrix),
	mWorldEulerAngles(original.GetWorldEulerAngles()), mLocalRotation(original.mLocalRotation), mWorldRotation(original.GetWorldRotation()), mLocalEulerAngles(original.mLocalEulerAngles),
	mWorldScale(original.GetWorldScale()), mLocalScale(original.mLocalScale), 
	mFront(original.mFront), mUp(original.mUp), mRight(original.mRight), mIsTransformModified(true), mUpdatedTransform(true),
	mPrefabId(original.mPrefabId), mIsPrefabOverride(original.mIsPrefabOverride), mIsDynamic(original.mIsDynamic)
{
	SetTag(original.mTag);
	for (Component* component : original.mComponents)
	{
		Component* cloned = component->Clone(this);
		mComponents.push_back(cloned);
		if (meshRendererComps && cloned->GetType() == ComponentType::MESHRENDERER)
		{
			meshRendererComps->push_back(static_cast<MeshRendererComponent*>(cloned));
		}
	}

	App->GetScene()->AddGameObjectToScene(this);
	for (GameObject* child : original.mChildren)
	{
		GameObject* gameObject = new GameObject(*child, this, originalToNew, meshRendererComps);
		AddChild(gameObject);
	}

	if (originalToNew)
	{
		(*originalToNew)[&original] = this;
	}
}

GameObject::~GameObject()
{
	App->GetScene()->RemoveGameObjectFromScene(this);
	App->GetScene()->DeleteFromTagMap(mTag, this);
	App->GetScriptManager()->RemoveGameObject(this);

	for (Component* component : mComponents)
	{
		delete component;
	}

	for (GameObject* gameObject : mChildren)
	{
		delete gameObject;
	}

	
}

#pragma endregion

#pragma region Base functions

void GameObject::Update()
{
	if (IsActive())
	{
		for (size_t i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update();
		}

		for (size_t i = 0; i < mChildren.size(); i++)
		{
			mChildren[i]->Update();
		}
	}

	if (mComponentsToDelete.size() > 0)
	{
		DeleteComponents();
	}
	if (mUpdatedTransform)
	{
		mUpdatedTransform = false;
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
		mixedAABB.Enclose(static_cast<MeshRendererComponent*>(component)->GetAABB());
	}

	return mixedAABB;
}


//Setters

void GameObject::SetTag(const std::string& tag) 
{
	App->GetScene()->DeleteFromTagMap(mTag, this);
	mTag = tag;
	App->GetScene()->AddToTagMap(tag, this);

	CameraComponent* camera = static_cast<CameraComponent*>(GetComponent(ComponentType::CAMERA));
	if (camera)
	{
		App->GetCamera()->AddMainCamera(camera);
	}
}

void GameObject::SetDynamic(bool dynamic)
{
	mIsDynamic = dynamic;
	for (GameObject* child : mChildren)
	{
		child->SetDynamic(dynamic);
	}
}

void GameObject::SetParent(GameObject* newParent)
{
	assert(newParent && "Parent was nullptr");
	mParent->RemoveChild(mUid);
	mParent = newParent;
	mParent->AddChild(this);
	if (mParent->GetWorldTransform().IsInvertible())
	{
		mLocalTransformMatrix = mParent->GetWorldTransform().Inverted().Mul(GetWorldTransform());
	}
	else
	{
		mLocalTransformMatrix = float4x4::identity;
	}
	float3 localPos;
	mLocalTransformMatrix.Decompose(localPos, mLocalRotation, mLocalScale);
	mLocalEulerAngles = mLocalRotation.ToEulerXYZ();
	SetTransformsDirtyFlag();

	SetActive(mParent->mIsActive && mIsEnabled);
}

void GameObject::SetEnabled(bool enabled)
{
	if (mIsEnabled != enabled)
	{
		mIsEnabled = enabled;
		if (!enabled || IsRoot() || mParent->IsActive())
		{
			SetActive(enabled);
		}
	}
}

void GameObject::SetActive(bool active)
{
	if (active && !mIsEnabled)
	{
		return;
	}

	mIsActive = active;

	for (unsigned int i = 0; i < mComponents.size(); ++i)
	{
		mComponents[i]->SetEnable(active);
	}

	for (unsigned int i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->SetActive(active);
	}
}

#pragma endregion

#pragma region Transform
void GameObject::RecalculateMatrices() const
{
	if (mIsTransformModified)
	{
		mWorldTransformMatrix = mLocalTransformMatrix;
		GameObject* parent = mParent;
		while (parent != nullptr)
		{
			mWorldTransformMatrix = parent->GetLocalTransform() * mWorldTransformMatrix;
			parent = parent->mParent;
		}

		mFront = (mWorldTransformMatrix * float4(float3::unitZ, 0)).xyz().Normalized();
		mUp = (mWorldTransformMatrix * float4(float3::unitY, 0)).xyz().Normalized();
		mRight = (mWorldTransformMatrix * float4(float3::unitX, 0)).xyz().Normalized();

		float3 trans;
		mWorldTransformMatrix.Decompose(trans, mWorldRotation, mWorldScale);
		mWorldEulerAngles = mWorldRotation.ToEulerXYZ();

		mIsTransformModified = false;
		mUpdatedTransform = true;
	}
}

void GameObject::SetTransformsDirtyFlag() const
{
	//if (mIsTransformModified)
	//	return;

	for (int i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->SetTransformsDirtyFlag();
	}
	mIsTransformModified = true;
}

//Position
void GameObject::SetWorldPosition(const float3& position)
{
	if (mParent && mParent->GetID() != App->GetScene()->GetRoot()->GetID())
	{
		assert(mParent->GetWorldTransform().IsInvertible());
		float3 pos = mParent->GetWorldTransform().Inverted().TransformPos(position);
		SetLocalPosition(pos);
	}
	else
	{
		SetLocalPosition(position);
	}
}

void GameObject::SetLocalPosition(const float3& position)
{
	mLocalTransformMatrix.SetTranslatePart(position);
	SetTransformsDirtyFlag();
}

void GameObject::SetWorldRotation(const float3& rotationInRadians)
{
	if (mParent && mParent->GetID() != App->GetScene()->GetRoot()->GetID())
	{
		float4x4 mat = mParent->GetWorldTransform();
		mat.ExtractScale();
		assert(mat.RotatePart().IsInvertible());
		SetLocalRotation(mat.RotatePart().Inverted().Mul(Quat::FromEulerYXZ(rotationInRadians.y, rotationInRadians.x, rotationInRadians.z)).ToQuat());
	}
	else
	{
		SetLocalRotation(rotationInRadians);
	}
}

void GameObject::SetWorldRotation(const Quat& rotation)
{
	if (mParent && mParent->GetID() != App->GetScene()->GetRoot()->GetID())
	{
		float4x4 mat = mParent->GetWorldTransform();
		mat.ExtractScale();
		assert(mat.RotatePart().IsInvertible());
		SetLocalRotation(mat.RotatePart().Inverted().Mul(rotation).ToQuat());
	}
	else
	{
		SetLocalRotation(rotation);
	}
}

void GameObject::SetLocalRotation(const float3& rotationInRadians)
{
	mLocalRotation = Quat::FromEulerYXZ(rotationInRadians.y, rotationInRadians.x, rotationInRadians.z);

	mLocalEulerAngles = rotationInRadians;
	mLocalTransformMatrix = float4x4::FromTRS(GetLocalPosition(), mLocalRotation, mLocalScale);
	SetTransformsDirtyFlag();
}

void GameObject::SetLocalRotation(const Quat& rotation)
{
	mLocalRotation = rotation;
	mLocalEulerAngles = rotation.ToEulerXYZ();
	mLocalTransformMatrix = float4x4::FromTRS(GetLocalPosition(), mLocalRotation, mLocalScale);
	SetTransformsDirtyFlag();
}

void GameObject::SetWorldScale(const float3& scale)
{
	if (mParent)
	{
		assert(mParent->GetWorldTransform().IsInvertible());
		SetLocalScale(scale.Div(mParent->GetWorldTransform().GetScale()));
	}
	else
	{
		SetLocalScale(scale);
	}
}

void GameObject::SetLocalScale(const float3& scale)
{
	float3 cleanScale = scale;
	if (scale.x == 0.0f) cleanScale.x = 0.0001f;
	if (scale.y == 0.0f) cleanScale.y = 0.0001f;
	if (scale.z == 0.0f) cleanScale.z = 0.0001f;

	mLocalScale = cleanScale;
	mLocalTransformMatrix = float4x4::FromTRS(GetLocalPosition(), mLocalRotation, mLocalScale);
	SetTransformsDirtyFlag();
}

void GameObject::Translate(const float3& translation)
{
	SetLocalPosition(GetLocalPosition() + translation);
}

void GameObject::LookAt(const float3& target)
{
	float4x4 rotationMatrix = float4x4::identity;

	float3 forward = -(target - GetWorldPosition()).Normalized();
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


	mLocalRotation = Quat(rotationMatrix);
	mWorldEulerAngles = mLocalRotation.ToEulerXYZ();


	mLocalTransformMatrix = float4x4::FromTRS(GetLocalPosition(), mLocalRotation, mWorldScale);
	SetTransformsDirtyFlag();
}

void GameObject::ResetTransform()
{
	mLocalRotation = Quat::identity;
	mWorldEulerAngles = float3(0.0f);
	mWorldScale = float3::one;
	mLocalTransformMatrix = float4x4::FromTRS(float3::zero, mLocalRotation, mWorldScale);
	SetTransformsDirtyFlag();
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
	case ComponentType::POINTLIGHT:
		newComponent = new PointLightComponent(this);
		break;
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
	case ComponentType::MASK:
		newComponent = new MaskComponent(this);
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
	case ComponentType::BOXCOLLIDER:
		newComponent = new BoxColliderComponent(this);
		break;
	case ComponentType::SLIDER:				//TODO: Redoo UI To not create gameObjects in a component	
		newComponent = new SliderComponent(this);
		break;
	case ComponentType::PARTICLESYSTEM:
		newComponent = new ParticleSystemComponent(this);
		break;
	case ComponentType::DECAL:
		newComponent = new DecalComponent(this);
		break;
	case ComponentType::TRAIL:
		newComponent = new TrailComponent(this);
		break;
	case ComponentType::TEXT:
		newComponent = new TextComponent(this);
		break;
	default:
		break;
	}

	if (newComponent)
	{
		mComponents.push_back(newComponent);
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

Component* GameObject::GetComponentInChildren(ComponentType type) const
{
	Component* gameObjectComponent = GetComponent(type);

	if (gameObjectComponent)
	{
		return gameObjectComponent;
	}

	for (GameObject* child : mChildren)
	{
		gameObjectComponent = child->GetComponentInChildren(type);

		if (gameObjectComponent)
		{
			return gameObjectComponent;
		}
	}

	return nullptr;
}

void GameObject::GetMeshesInChildren(std::vector<const MeshRendererComponent*>& componentVector) const
{
	MeshRendererComponent* gameObjectComponent = static_cast<MeshRendererComponent*>(GetComponent(ComponentType::MESHRENDERER));

	if (gameObjectComponent)
	{
		componentVector.push_back(gameObjectComponent);
	}

	for (GameObject* child : mChildren)
	{
		child->GetMeshesInChildren(componentVector);
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

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}

void GameObject::DeleteComponents()
{
	for (std::vector<Component*>::iterator deleteIt = mComponentsToDelete.begin(); deleteIt != mComponentsToDelete.end(); ++deleteIt)
	{
		for (std::vector<Component*>::iterator compIt = mComponents.begin(); compIt != mComponents.end(); ++compIt)
		{
			if ((*compIt)->GetType() == (*deleteIt)->GetType())
			{	
				mComponents.erase(compIt);
				break;
			}
		}
		delete *deleteIt;
	}
	mComponentsToDelete.clear();
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
	obj.AddBool("Active", mIsActive);
	obj.AddFloats("Translation", GetLocalPosition().ptr(), 3);
	obj.AddFloats("Rotation", mLocalRotation.ptr(), 4);
	obj.AddFloats("Scale", mLocalScale.ptr(), 3);
	obj.AddString("Tag", mTag.c_str());
	obj.AddInt("PrefabUid", mPrefabId);
	obj.AddBool("OverridePrefab", mIsPrefabOverride);
	obj.AddBool("Dynamic", mIsDynamic);

	// Save components
	JsonArray components = obj.AddNewJsonArray("Components");
	for (const Component* component : mComponents)
	{
		JsonObject comp = components.PushBackNewObject();
		component->Save(comp);
	}
}

void GameObject::LoadGameObject(const JsonObject& jsonObject, std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	mIsEnabled = jsonObject.GetBool("Enabled");
	float pos[3]; 

	if(jsonObject.HasMember("Active"))
	{
		mIsActive = jsonObject.GetBool("Active");
	}

	jsonObject.GetFloats("Translation", pos);
	SetLocalPosition(float3(pos));
	float rot[4]; 
	jsonObject.GetFloats("Rotation", rot);
	SetLocalRotation(Quat(rot));
	float scale[3]; 
	jsonObject.GetFloats("Scale", scale);
	SetLocalScale(float3(scale));

	SetTag(jsonObject.GetString("Tag"));
	mPrefabId = jsonObject.GetInt("PrefabUid");
	mIsPrefabOverride = jsonObject.GetBool("OverridePrefab");
	mIsDynamic = jsonObject.GetBool("Dynamic");
	uidPointerMap[mUid] = this;
}

void GameObject::LoadComponents(const JsonObject& jsonObject, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	// Load components
	JsonArray components = jsonObject.GetJsonArray("Components");
	for (unsigned int i = 0; i < components.Size(); ++i)
	{
		JsonObject componentData = components.GetJsonObject(i);
		ComponentType cType = (ComponentType)componentData.GetInt("ComponentType");
		Component* component = this->CreateComponent(cType);
		component->Load(componentData, uidPointerMap);
	}
}
#pragma endregion

#pragma region Children

void GameObject::AddChild(GameObject* child)
{
	mChildren.push_back(child);
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* object = nullptr;
	for (std::vector<GameObject*>::const_iterator it = mChildren.cbegin(); it != mChildren.cend(); ++it)
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
