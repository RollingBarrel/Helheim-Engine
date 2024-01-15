#pragma once
#include <vector>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"
#include "Component.h"
//#include "MathGeoLib.h"

class GameObject
{
	friend class HierarchyPanel;
	friend class InspectorPanel;
public:
	GameObject(GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	GameObject(const char* name, GameObject* parent);
	GameObject(const char* name, GameObject* parent, float3 position, float3 scale, Quat rotation);

	~GameObject();

	//template<class T>
	//T* GetComponent();
	Component* GetComponent(ComponentType type);
	void RecalculateMatrices();
	void Update();
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };
	void AddChild(GameObject* child, const int aboveThisId = 0);
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const float3& GetRotation() const { return mRotation; }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mScale; }
	GameObject* GetParent() const { return mParent; }
	const std::string* GetName() const { return &mName; }
	void ResetTransform();

	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	const unsigned int GetID() const { return mID; }
	const bool IsRoot() const { return mIsRoot; }
	void DeleteChild(GameObject* child);
	void AddComponentToDelete(Component* component);

	void SetRotation(const float3& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);

	void CreateComponent(ComponentType type);

private:
	GameObject* RemoveChild(const int id);
	void AddSuffix();
	void DeleteComponents();
	Component* RemoveComponent(Component* component);
	void AddComponent(Component* component, Component* position);
	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;
	std::vector<Component*> mComponents;
	std::vector<Component*> mComponentsToDelete;
	const unsigned int mID;
	std::string mName = "Game Object";
	float4x4 mWorldTransformMatrix = float4x4::identity;
	float4x4 mLocalTransformMatrix = float4x4::identity;
	const bool mIsRoot = false;
	float3 mPosition = float3::zero;
	float3 mRotation = float3::zero;
	float3 mScale = float3::one;
	bool mIsEnabled = true;
};

