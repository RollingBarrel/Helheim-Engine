#pragma once
#include <vector>

class Component;

class GameObject
{

public:

	void RecalculateMatrices();
	void Update();
	void CreateComponent();
	void DrawEditor();
	void Enable() { mIsEnabled = true; };
	void Disable() { mIsEnabled = false; };


private:
	std::vector<GameObject*> mChildren;
	GameObject* mParent;
	std::vector<Component*> mComponents;
	const unsigned int mID;
	char* mName;
	float4x4 mWorldTransformMatrix;
	float4x4 mLocalTransformMatrix;
	bool mIsRoot;
	float3 mPosition;
	Quat mRotation;
	float3 mScale;
	bool mIsEnabled;


};

