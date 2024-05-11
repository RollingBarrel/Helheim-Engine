#pragma once
#include <vector>
#include "Globals.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"
#include "Archive.h"
#include "Tag.h"
#include "MeshRendererComponent.h"

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"

class AIAgentComponent;
class CameraComponent;
class Component;
enum class ComponentType : unsigned int;

class ENGINE_API GameObject
{
	friend class HierarchyPanel;
	friend class InspectorPanel;

public:
	GameObject(GameObject* parent);
	GameObject(const char* name, GameObject* parent);
	GameObject(unsigned int ID, const char* name, GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	~GameObject();

	void Update();

	// Getters
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const float3& GetRotation() const { return mEulerRotation; }
	const Quat& GetRotationQuat() const { return mRotation; }
	float3 GetWorldPosition() const { return mWorldTransformMatrix.TranslatePart(); }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mScale; }
	GameObject* GetParent() const { return mParent; }
	const std::string& GetName() const { return mName; }
	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	float3 GetFront() const { return ( mWorldTransformMatrix * float4(float3::unitZ, 0)).xyz().Normalized(); } 
	float3 GetUp() const { return (mWorldTransformMatrix * float4(float3::unitY, 0)).xyz().Normalized(); }
	float3 GetRight() const { return (mWorldTransformMatrix * float4(float3::unitX, 0)).xyz().Normalized(); }
	Tag* GetTag() const { return mTag; }
	unsigned int GetID() const { return mID; }
	bool IsRoot() const { return mIsRoot; }
	// Status for this GameObject
	bool IsEnabled() const { return mIsEnabled; }
	// Status for this GameObject and all its ancestors
	bool IsActive() const { return mIsEnabled && mIsActive; }
	
	// Children
	void AddChild(GameObject* child, const int aboveThisId = 0);
	GameObject* RemoveChild(const int id);
	void DeleteChild(GameObject* child);

	// Setters
	void SetRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);
	void SetTag(Tag* tag) { mTag = tag; };
	void SetName(const char* name) { mName = name; };
	void SetEnabled(bool enabled);

	// Transform
	const bool HasUpdatedTransform() const;
	void LookAt(float3 target);
	void ResetTransform();
	void RecalculateMatrices();

	// Finds
	GameObject* Find(const char* name) const;
	GameObject* Find(unsigned int UID) const;
	static GameObject* FindGameObjectWithTag(std::string tagname);
	static std::vector<GameObject*> FindGameObjectsWithTag(std::string tagname);

	// Components
	Component* CreateComponent(ComponentType type);
	Component* GetComponent(ComponentType type) const;
	std::vector<Component*> GetComponents(ComponentType type) const;
	std::vector<Component*> GetComponentsInChildren(ComponentType type) const;
	Component* GetComponentInParent(ComponentType type) const;
	std::vector<Component*> FindComponentsInChildren(GameObject* parent, const ComponentType type);
	const AnimationComponent* FindAnimationComponent();
	void AddComponent(Component* component, Component* position);
	void AddComponentToDelete(Component* component);

	// Save / Load
	void Save(Archive& archive, int parentId) const;
	void Load(const rapidjson::Value& gameObjectsJson);
	static GameObject* LoadGameObjectFromJSON(const rapidjson::Value& gameObject, GameObject* parent);
	void LoadComponentsFromJSON(const rapidjson::Value& components);

	// Prefabs
	void LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id);
	void SetPrefabId(unsigned int id) { mPrefabResourceId = id; }
	void SetPrefabOverride(bool ov) { mPrefabOverride = ov; }

private:
	void AddSuffix();

	void DeleteComponents();
	Component* RemoveComponent(Component* component);

	void RecalculateLocalTransform();
	void RefreshBoundingBoxes();
	void SetActiveInHierarchy(bool active);
	std::pair<GameObject*, int> RecursiveTreeSearch(GameObject* owner, std::pair<GameObject*, int> currentGameObject, const int objectToFind);

	const unsigned int mID;
	std::string mName = "GameObject";

	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;

	// Components
	std::vector<Component*> mComponents;
	std::vector<Component*> mComponentsToDelete;
	Tag* mTag = nullptr;

	// Transform
	float4x4 mWorldTransformMatrix = float4x4::identity;
	float4x4 mLocalTransformMatrix = float4x4::identity;
	float3 mPosition = float3::zero;
	Quat mRotation = Quat::identity;
	float3 mEulerRotation = float3::zero;
	float3 mScale = float3::one;

	// Prefabs
	int mPrefabResourceId = 0;
	bool mPrefabOverride = true;

	bool mIsEnabled = true;
	bool mIsActive = true;
	bool mIsTransformModified = false;
	const bool mIsRoot = false;
};
