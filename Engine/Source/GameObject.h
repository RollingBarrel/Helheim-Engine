#pragma once
#include <vector>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"
#include "Archive.h"
#include "Component.h"

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
	GameObject(const char* name, unsigned int id, GameObject* parent, float3 position, float3 scale, Quat rotation);


	~GameObject();

	Component* GetComponent(ComponentType type);
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
	const float3& GetRotation() const { return mLocalTransformMatrix.ToEulerXYZ(); }
	const float3& GetPosition() const { return mLocalTransformMatrix.TranslatePart(); }
	const float3& GetScale() const { return mLocalTransformMatrix.GetScale(); }
	GameObject* GetParent() const { return mParent; }
	const std::string* GetName() const { return &mName; }
	void ResetTransform();

	const std::vector<GameObject*>& GetChildren() const { return mChildren; }
	const unsigned int GetID() const { return mID; }
	const bool IsRoot() const { return mIsRoot; }
	void DeleteChild(GameObject* child);
	void AddComponentToDelete(Component* component);

	void SetRotation(const float3& rotation);
	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);
	void SetID(const unsigned int ID);

	Component* CreateComponent(ComponentType type);
	void DeletePopup(Component* component, int headerPosition);

	void Save(Archive& archive) const;
	void Load(const rapidjson::Value& gameObjectsJson);

	MeshRendererComponent* getMeshRenderer() const;

private:
	GameObject* RemoveChild(const int id);
	void AddSuffix();
	void DeleteComponents();
	void RecalculateLocalTransform();
	void MoveChild(const int id, GameObject* newParent, const int aboveThisId = 0);
	void AddSufix();
	void DragAndDrop();
	std::vector<GameObject*> mChildren;
	GameObject* mParent = nullptr;
	std::vector<Component*> mComponents;
	std::vector<Component*> mComponentsToDelete;
	unsigned int mID;
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
	
	void DrawTransform();
	void AddComponentButton();
	void ShowComponents(Component* component);
	void DrawMeshRenderer(Component* component);
	void DrawMaterial(Component* component);
	void RemoveComponent(Component* component);
};

