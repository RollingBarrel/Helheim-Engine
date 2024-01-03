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
public:
	GameObject(GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	GameObject(const char* name, GameObject* parent);
	GameObject(const char* name, GameObject* parent, float3 position, float3 scale, Quat rotation);

	~GameObject();

	void RecalculateMatrices();
	void Update();
	void DrawInspector();
	void DrawHierarchy(const int selected);
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };
	void OnRightClick();
	void AddChild(GameObject* child, const int aboveThisId = 0);
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const Quat& GetRotation() const { return mRotation; }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mScale; }
	GameObject* GetParent() const { return mParent; }
	const std::string* GetName() const { return &mName; }

	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	const unsigned int GetID() const { return mID; }
	const bool IsRoot() const { return mIsRoot; }
	void DeleteChild(GameObject* child);

	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);

	void CreateComponent(ComponentType type);
	void DeletePopup(Component* component, int headerPosition);

private:
	void MoveChild(const int id, GameObject* newParent, const int aboveThisId = 0);
	void AddSufix();
	void DragAndDrop();
	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;
	std::vector<Component*> mComponents;
	const unsigned int mID;
	std::string mName;
	float4x4 mWorldTransformMatrix;
	float4x4 mLocalTransformMatrix;
	const bool mIsRoot;
	float3 mPosition;
	Quat mRotation;
	float3 mScale;
	bool mIsEnabled;

	int componentIndex;

	void DrawTransform();
	void AddComponentButton();
	void ShowComponents(Component* component);
	void DrawMeshRenderer(Component* component);
	void DrawMaterial(Component* component);
	void RemoveComponent(Component* component);
};

