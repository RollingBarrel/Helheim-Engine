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
#include "Archive.h"

class Component;
class MeshRendererComponent;
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
	const float3& GetFront() const { return mFront; }
	const float3& GetUp() const { return mUp; }
	const float3& GetRight() const { return mRight; }
	const std::string& GetTag() const { return mTag; }
	AABB GetAABB();
	bool IsDynamic() const { return mIsDynamic; }
	bool IsRoot() const { return mIsRoot; }
	bool IsEnabled() const { return mIsEnabled; } // Status for this GameObject
	bool IsActive() const { return mIsEnabled && mIsActive; } 	// Status for this GameObject and all its ancestors

	//Setters
	void SetTag(const std::string& tag);;
	void SetName(const char* name) { mName = name; };
	void SetDynamic(bool dynamic) { mIsDynamic = dynamic; };
	void SetParent(GameObject* newParent);
	void SetEnabled(bool enabled);

	// Children
	void AddChild(GameObject* child);
	GameObject* RemoveChild(const int id);	//Remove from mChildren does not delete

	//Transform
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	void RecalculateMatrices();															//TODO: REVIEW METHOD

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
	void Translate(const float3& translation);
	void LookAt(const float3& target);
	void ResetTransform();

	// Components
	template<typename T> T* CreateComponent();
	Component* CreateComponent(ComponentType type);
	template<typename T> T* GetComponent() const;
	Component* GetComponent(ComponentType type) const;
	template<typename T> T* GetComponentInParent() const;											
	Component* GetComponentInParent(ComponentType type) const;											
	void GetComponentsInChildren(ComponentType type, std::vector<Component*>& componentVector) const;
	//TODO: do the template correctly,SRRY
	void GetMeshesInChildren(std::vector<const MeshRendererComponent*>& componentVector) const;
	void AddComponentToDelete(Component* component);

	// Save / Load
	void Save(JsonObject& obj) const;
	void Load(const JsonObject& obj);

	// Prefabs
	void LoadChangesPrefab(const rapidjson::Value& gameObject, unsigned int id);   //TODO: This need to be here? 
	void SetPrefabId(unsigned int id) { mPrefabId = id; }						   //TODO: This need to be here?
	void SetPrefabOverride(bool override) { mPrefabOverride = override; }		   //TODO: This need to be here?
																				   
private:

	void DeleteComponents();
	void SetActive(bool active); 

	const unsigned int mUid;
	std::string mName = "GameObject";

	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;

	// Transform
	float4x4 mWorldTransformMatrix = float4x4::identity;
	float4x4 mLocalTransformMatrix = float4x4::identity;

	//Direction
	float3 mFront;
	float3 mUp;
	float3 mRight;

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
