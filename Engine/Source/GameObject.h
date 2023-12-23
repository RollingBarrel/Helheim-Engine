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
	GameObject(const GameObject* parent);
	GameObject(const char* name, const GameObject* parent);
	GameObject(const char* name, const GameObject* parent, float3 position, float3 scale, Quat rotation);
	void RecalculateMatrices();
	void Update();
	void CreateComponent();
	void DrawInspector();
	void DrawHierarchy();
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };
	void AddChild(GameObject* child);
	
	const float4x4& GetWorldTransform() const { return mWorldTransformMatrix; }
	const float4x4& GetLocalTransform() const { return mLocalTransformMatrix; }
	const Quat& GetRotation() const { return mRotation; }
	const float3& GetPosition() const { return mPosition; }
	const float3& GetScale() const { return mScale; }

	void SetRotation(const Quat& rotation);
	void SetPosition(const float3& position);
	void SetScale(const float3& scale);

private:
	std::vector<GameObject*> mChildren;
	const GameObject* mParent = nullptr;
	std::vector<Component*> mComponents;
	const unsigned int mID;
	std::string mName;
	float4x4 mWorldTransformMatrix;
	float4x4 mLocalTransformMatrix;
	bool mIsRoot;
	float3 mPosition;
	Quat mRotation;
	float3 mScale;
	bool mIsEnabled;


	void DrawTransform();

};

