#pragma once
#include <vector>
#include "Globals.h"
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"
#include "Archive.h"

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"

class MeshRendererComponent;
class CameraComponent;
class Component;
class Tag;
enum class ComponentType : unsigned int;

class ENGINE_API GameObject
{
	friend class HierarchyPanel;
	friend class InspectorPanel;

public:
	GameObject(GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	GameObject(const char* name, GameObject* parent);
	GameObject(const char* name, unsigned int id, GameObject* parent, float3 position, float3 scale, Quat rotation);

	~GameObject();

	Component* GetComponent(ComponentType type);
	void RecalculateMatrices();
	void Update();
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const float3& GetRotation() const { return mLocalTransformMatrix.ToEulerXYZ(); }
	const float3& GetWorldPosition() const { return mWorldTransformMatrix.TranslatePart(); }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mLocalTransformMatrix.GetScale(); }
	GameObject* GetParent() const { return mParent; }
	const std::string& GetName() const { return mName; }
	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	const float3& GetFront() const { return ( mWorldTransformMatrix * float4(float3::unitZ, 0)).xyz().Normalized(); }
	Tag* GetTag() const { return mTag; }

	void ResetTransform();

	void SetEnabled(bool enabled);
	// Status for this GameObject
	bool IsEnabled() const { return mIsEnabled; }
	// Status for this GameObject and all its ancestors
	bool IsActive() const { return mIsEnabled && mIsActive; }
	
	unsigned int GetID() const { return mID; }
	bool IsRoot() const { return mIsRoot; }
	void AddChild(GameObject* child, const int aboveThisId = 0);
	void DeleteChild(GameObject* child);
	void AddComponentToDelete(Component* component);

	void SetRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);
	void SetTag(Tag* tag) { mTag = tag; };

	GameObject* Find(const char* name);
	GameObject* Find(unsigned int UID);

	Component* CreateComponent(ComponentType type);
	MeshRendererComponent* GetMeshRenderer() const;
	CameraComponent* getCamera() const;
	void Save(Archive& archive) const;
	void Load(const rapidjson::Value& gameObjectsJson);

	static GameObject* FindGameObjectWithTag(std::string tagname);
	static std::vector<GameObject*> FindGameObjectsWithTag(std::string tagname);

private:
	GameObject* RemoveChild(const int id);
	void AddSuffix();
	void DeleteComponents();
	Component* RemoveComponent(Component* component);
	void AddComponent(Component* component, Component* position);
	void RecalculateLocalTransform();
	void RefreshBoundingBoxes();

	void SetActiveInHierarchy(bool active);

	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;
	Tag* mTag = nullptr;
	std::vector<Component*> mComponents;
	std::vector<Component*> mComponentsToDelete;
	const unsigned int mID;
	std::string mName = "GameObject";
	float4x4 mWorldTransformMatrix = float4x4::identity;
	float4x4 mLocalTransformMatrix = float4x4::identity;
	const bool mIsRoot = false;
	float3 mPosition = float3::zero;
	Quat mRotation = Quat::identity;
	float3 mEulerRotation = float3::zero;
	float3 mScale = float3::one;
	bool mIsEnabled = true;
	bool mIsActive = true;
	bool isTransformModified = false;
};

