#pragma once
#include "Globals.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "Math/float4.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "MeshRendererComponent.h"
#include "Archive.h"

class AIAgentComponent;
class CameraComponent;
class Component;
enum class ComponentType : unsigned int;

class ENGINE_API GameObject
{
	friend class HierarchyPanel;
	friend class InspectorPanel;

public:
	GameObject() = delete;
	GameObject(const GameObject& original) = delete;
	GameObject& operator=(const GameObject& other) = delete;

	explicit GameObject(const char* name);
	explicit GameObject(GameObject* parent);
	GameObject(const char* name, GameObject* parent);
	GameObject(unsigned int ID, const char* name, GameObject* parent);
	GameObject(const GameObject& original, GameObject* newParent);
	~GameObject();

	void Update();

	//Getters
	GameObject* GetParent() const { return mParent; }
	const std::string& GetName() const { return mName; }
	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	unsigned int GetID() const { return mUid; }
	float3 GetFront() const { return ( mWorldTransformMatrix * float4(float3::unitZ, 0)).xyz().Normalized(); } 
	float3 GetUp() const { return (mWorldTransformMatrix * float4(float3::unitY, 0)).xyz().Normalized(); }
	float3 GetRight() const { return (mWorldTransformMatrix * float4(float3::unitX, 0)).xyz().Normalized(); }
	const std::string& GetTag() const { return mTag; }
	AABB GetAABB();
	bool IsDynamic() const { return mIsDynamic; }

	//Setters
	void SetTag(std::string tag) { mTag = tag; };
	void SetName(const char* name) { mName = name; };
	void SetDynamic(bool dynamic) { mIsDynamic = dynamic; };

	void SetParent(GameObject* newParent);

	bool IsRoot() const { return mIsRoot; }
	// Status for this GameObject
	void SetEnabled(bool enabled);
	bool IsEnabled() const { return mIsEnabled; }
	// Status for this GameObject and all its ancestors
	bool IsActive() const { return mIsEnabled && mIsActive; }
	
	// Children
	void AddChild(GameObject* child);
	void RemoveChild(const int id);

	//Transform
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	void RecalculateMatrices();

	//Position
	const float3& GetPosition() const { return mPosition; }
	const float3& GetLocalPosition() const { return mLocalPosition; }
	void SetPosition(const float3& position);
	void SetLocalPosition(const float3& position);
	//Rotation
	const Quat& GetRotation() const { return mRotation; }
	const Quat& GetLocalRotation() const { return mLocalRotation; }
	const float3& GetEulerAngles() const { return mEulerAngles; }
	const float3& GetLocalEulerAngles() const { return mLocalEulerAngles; }
	void SetRotation(const float3& rotation);
	void SetLocalRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetLocalRotation(const Quat& rotation);
	//Scale
	const float3& GetScale() const { return mScale; }
	const float3& GetLocalScale() const { return mLocalScale; }
	void SetScale(const float3& scale);
	void SetLocalScale(const float3& scale);

	// Transform
	const bool HasUpdatedTransform() const;

	void Translate(float3 translation);
	void LookAt(float3 target);
	void ResetTransform();

	// Finds
	GameObject* Find(const char* name) const;
	GameObject* Find(unsigned int UID) const;
	std::vector<Component*>& FindComponentsInChildren(GameObject* parent, const ComponentType type);

	// Components
	template<typename T> T* CreateComponent();
	Component* CreateComponent(ComponentType type);
	template<typename T> T* GetComponent() const;
	Component* GetComponent(ComponentType type) const;

	void GetComponentsInChildren(ComponentType type, std::vector<Component*>& componentVector) const;	//TODO: template
	Component* GetComponentInParent(ComponentType type) const;											//TODO: template

	void AddComponent(Component* component, Component* position);	//TODO: DELETE FAST PLS
	void AddComponentToDelete(Component* component);

	// Save / Load
	void Save(JsonObject& obj) const;
	void Load(const JsonObject& obj);

	// Prefabs
	void LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id);
	void SetPrefabId(unsigned int id) { mPrefabId = id; }
	void SetPrefabOverride(bool ov) { mPrefabOverride = ov; }

private:
	void AddSuffix();

	void DeleteComponents();
	Component* RemoveComponent(Component* component);

	void SetActiveInHierarchy(bool active);

	const unsigned int mUid;
	std::string mName = "GameObject";

	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;

	// Transform
	float4x4 mWorldTransformMatrix = float4x4::identity;
	float4x4 mLocalTransformMatrix = float4x4::identity;
	//Position
	float3 mPosition = float3::zero;
	float3 mLocalPosition = float3::zero;
	//Rotation
	Quat mRotation = Quat::identity;
	Quat mLocalRotation = Quat::identity;
	//Rotation in Euler Angles (Rads Always)
	float3 mEulerAngles = float3::zero;
	float3 mLocalEulerAngles = float3::zero;
	//Scale
	float3 mScale = float3::one;
	float3 mLocalScale = float3::one;

	// Components
	std::vector<Component*> mComponents;
	std::vector<Component*> mComponentsToDelete;

	// Prefabs
	int mPrefabId = 0;
	bool mPrefabOverride = true;

	std::string mTag = "Untagged";

	bool mIsEnabled = true;
	bool mIsActive = true;
	bool mIsDynamic = false;
	bool mIsTransformModified = false;
	const bool mIsRoot = false;
};
