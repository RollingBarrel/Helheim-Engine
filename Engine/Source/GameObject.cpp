#include "GameObject.h"
#include "Algorithm/Random/LCG.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"
#include <algorithm>

#include "MeshRendererComponent.h"
#include "TestComponent.h"
#include <MathFunc.h>

GameObject::GameObject(GameObject* parent)
	:mID(LCG().Int()), mName("GameObject"), mParent(parent),
	mIsRoot(parent == nullptr)
{
	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

	AddSuffix();
}

GameObject::GameObject(const GameObject& original)
	:mID(LCG().Int()), mName(original.mName), mParent(original.mParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	AddSuffix();

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const GameObject& original, GameObject* newParent)
	:mID(LCG().Int()), mName(original.mName), mParent(newParent),
	mIsRoot(original.mIsRoot), mIsEnabled(original.mIsEnabled), mWorldTransformMatrix(original.mWorldTransformMatrix),
	mLocalTransformMatrix(original.mLocalTransformMatrix), mPosition(original.mPosition), mScale(original.mScale),
	mRotation(original.mRotation)
{

	for (auto child : original.mChildren) {
		GameObject* gameObject = new GameObject(*(child), this);
		gameObject->mParent = this;
		mChildren.push_back(gameObject);
	}

	for (auto component : original.mComponents) {
		mComponents.push_back(component->Clone(this));
	}
}

GameObject::GameObject(const char* name, GameObject* parent)
	:mID(LCG().Int()), mName(name), mParent(parent),
	mIsRoot(parent == nullptr)
{

	if (!mIsRoot) {
		mWorldTransformMatrix = mParent->GetWorldTransform();
	}

}

GameObject::~GameObject()
{
	for (GameObject* gameObject : mChildren) {
		delete gameObject;
		gameObject = nullptr;
	}
	mChildren.clear();
	for (Component* component : mComponents) {
		delete component;
		component = nullptr;
	}
	mComponents.clear();

}


/*template<class T>
T* GameObject::GetComponent() {
	T& GameObject::GetComponent() {
		for (auto&& component : components) {
			if (component->IsClassType(T::Type))
				return *static_cast<T*>(component.get());
		}

		return *std::unique_ptr<T>(nullptr);
	}
}
*/
Component* GameObject::GetComponent(ComponentType type)
{
	for (auto component : mComponents) {
		if (component->GetType() == type) {
			return component;
		}
	}
	return nullptr;
}

void GameObject::RecalculateMatrices()
{
	mLocalTransformMatrix = float4x4::FromTRS(mPosition, Quat::FromEulerXYZ(DegToRad(mRotation.x), DegToRad(mRotation.y), DegToRad(mRotation.z)), mScale);

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

	DeleteComponents();
	ReorderComponents();
}

void GameObject::ResetTransform()
{
	mPosition = { 0,0,0 };
	mScale = { 1,1,1 };
	mRotation = { 0,0,0 };
}

void GameObject::DeleteChild(GameObject* child)
{
	RemoveChild(child->mID);
	delete child;
	child = nullptr;
}

void GameObject::AddComponentToDelete(Component* component)
{
	mComponentsToDelete.push_back(component);
}


void GameObject::SetRotation(const float3& rotation)
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

void GameObject::AddChild(GameObject* child, const int aboveThisId)
{
	child->mParent = this;
	bool inserted = false;
	if (aboveThisId != 0) {
		for (auto it = mChildren.cbegin(); it != mChildren.cend(); ++it) {
			if ((*it)->GetID() == aboveThisId)
			{
				mChildren.insert(it, child);
				inserted = true;
				break;
			}
		}
	}

	child->mLocalTransformMatrix = mWorldTransformMatrix.Inverted() * child->mWorldTransformMatrix;

	if (!inserted) {
		mChildren.push_back(child);
	}
}

GameObject* GameObject::RemoveChild(const int id)
{
	GameObject* movedObject = nullptr;
	std::vector<GameObject*>::iterator itTargetPosition = mChildren.end();
	for (auto it = mChildren.begin(); it != mChildren.cend(); ++it) {
		if ((*it)->GetID() == id) {
			movedObject = *it;
			mChildren.erase(it);
			break;
		}
	}
	return movedObject;

}

void GameObject::AddSuffix()
{
	bool found = true;
	int count = 1;
	int last_pos = -1;
	while (found) {
		std::string str = " (" + std::to_string(count) + ')';
		int pos = std::string::npos;

		std::string nameWithSufix = mName + str;
		for (auto gameObject : mParent->mChildren)
		{
			if (pos == -1) {
				//pos = gameObject->mName.find(str, gameObject->mName.size() - 4);
				pos = gameObject->mName.find(nameWithSufix);
			}

		}

		if (pos == std::string::npos) {
			if (mParent->mChildren.size() > 0) {
				mName += str;
			}
			found = false;
		}
		else {
			count++;
			last_pos = pos;
		}

	}
}

void GameObject::CreateComponent(ComponentType type) {
	Component* newComponent = nullptr;

	switch (type) {
	case ComponentType::MESHRENDERER:
		newComponent = new MeshRendererComponent(this);
		break;
	case ComponentType::TEST:
		newComponent = new TestComponent(this);
		break;
	default:
		break;
	}

	if (newComponent) {
		mComponents.push_back(newComponent);
	}
}

void GameObject::DeleteComponents() {
	for (auto component : mComponentsToDelete)
	{
		auto it = std::find(mComponents.begin(), mComponents.end(), component);
		if (it != mComponents.end()) {
			mComponents.erase(it);
			delete component;
			component = nullptr;
		}
	}
}

Component* GameObject::RemoveComponent(Component* component)
{
	Component* movedComponent = nullptr;
	auto it = std::find(mComponents.begin(), mComponents.end(), component);
	if (it != mComponents.end()) {
		movedComponent = *it;
		mComponents.erase(it);		
	}
	return movedComponent;
}

void GameObject::AddComponent(Component* component, Component* position)
{
	if (position == nullptr) {
		mComponents.push_back(component);
	}
	else {
		auto it = std::find(mComponents.begin(), mComponents.end(), position);
		mComponents.insert(it, component);
	}
}

void GameObject::ReorderComponents() {
	if (mComponentsToSwap.first != nullptr) {
		RemoveComponent(mComponentsToSwap.first);
		AddComponent(mComponentsToSwap.first, mComponentsToSwap.second);
		mComponentsToSwap = { nullptr, nullptr };
	}
}