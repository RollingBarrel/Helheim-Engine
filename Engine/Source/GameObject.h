#pragma once
#include <vector>
#include "Math/float4x4.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include "string"
//#include "MathGeoLib.h"

class Component;

class GameObject
{

public:
	GameObject(GameObject* parent);
	GameObject(const GameObject& original);
	GameObject(const GameObject& original, GameObject* newParent);
	GameObject(const char* name, GameObject* parent);
	GameObject(const char* name, GameObject* parent, float3 position, float3 scale, Quat rotation);
	void RecalculateMatrices();
	void Update();
	
	//void CreateComponent();
	
	void DrawInspector();
	void DrawHierarchy(const int selected);
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };
	void AddChild(GameObject* child, const int aboveThisId = 0);
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const Quat& GetRotation() const { return mRotation; }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mScale; }
	const int GetID() const { return mID; }
	GameObject* GetParent() const { return mParent; }
	const std::string* GetName() const { return &mName; }

	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);

	//***************************************************************************

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	//***************************************************************************

private:
	void MoveChild(const int id, GameObject* newParent, const int aboveThisId = 0);
	void AddSufix();
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

	void DrawTransform();
	
	//*******************************************************
	int componentIndex;

	int componentLines = 1; //Start by counting the header
	bool hasMeshRenderer = false;
	bool hasMaterial = false;

	void AddComponentButton();
	void ShowComponents(Component* component);
	void DrawMeshRenderer(Component* component);
	void DrawMaterial(Component* component);
	void DeletePopup(Component* component);
	//*******************************************************
};

