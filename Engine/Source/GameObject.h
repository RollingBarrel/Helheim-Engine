#pragma once
#include <vector>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"

class MeshRendererComponent;
class Component;
enum class ComponentType : unsigned int;

class GameObject
{
	friend class HierarchyPanel;
	friend class InspectorPanel;

public:
	GameObject(GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	GameObject(const char* name, GameObject* parent);

	~GameObject();

	Component* GetComponent(ComponentType type);
	void RecalculateMatrices();
	void Update();
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };
	void AddChild(GameObject* child, const int aboveThisId = 0);
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const float3& GetRotation() const { return mLocalTransformMatrix.ToEulerXYZ(); }
	const float3& GetPosition() const { return mLocalTransformMatrix.TranslatePart(); }
	const float3& GetScale() const { return mLocalTransformMatrix.GetScale(); }
	GameObject* GetParent() const { return mParent; }
	const std::string& GetName() const { return mName; }
	void ResetTransform();

	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	unsigned int GetID() const { return mID; }
	bool IsRoot() const { return mIsRoot; }
	void DeleteChild(GameObject* child);
	void AddComponentToDelete(Component* component);

	void SetRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);

	void CreateComponent(ComponentType type);
	MeshRendererComponent* getMeshRenderer() const;

private:
	GameObject* RemoveChild(const int id);
	void AddSuffix();
	void DeleteComponents();
	Component* RemoveComponent(Component* component);
	void AddComponent(Component* component, Component* position);
	void RecalculateLocalTransform();

	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;
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
	bool isTransformModified = false;
	
};

