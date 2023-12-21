#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "MathGeoLib.h"

GameObject::GameObject(const char* name, const GameObject* parent) 
	:mID(2), mName(name), mParent(parent), 
	mIsRoot(false), mIsEnabled(true), mWorldTransformMatrix(float4x4::zero), 
	mLocalTransformMatrix(float4x4::zero), mPosition(float3::zero), mScale(float3::zero), 
	mRotation(Quat::identity)
{

	if (parent == nullptr) {
		mIsRoot = true;

	}
	else {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}


}

void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, mRotation, mScale);

	mWorldTransformMatrix = mParent->GetWorldTransform() * mLocalTransformMatrix;

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->RecalculateMatrices();
	}

}

void GameObject::Update()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		mComponents[i]->Update();
	}

	for (size_t i = 0; i < mChildren.size(); i++) {
		mChildren[i]->Update();
	}
}

void GameObject::SetRotation(const Quat& rotation)
{
	mRotation = rotation;
	RecalculateMatrices();
}

void GameObject::SetPosition(const float3& position)
{
	mPosition = position;
	RecalculateMatrices();
}

void GameObject::SetScale(const float3& scale)
{
	mScale = scale;
	RecalculateMatrices();
}

