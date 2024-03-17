#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"

#include "AnimationComponent.h"

#include "GameObject.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION), mAnimation(nullptr), mController(nullptr)
{
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION)
{
	mAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(other.mAnimation->GetUID(), Resource::Type::Mesh));

	mController = other.mController;
}

AnimationComponent::~AnimationComponent()
{
	delete mController;
	mController = nullptr;

	if (mAnimation)
	{
		App->GetResource()->ReleaseResource(mAnimation->GetUID());
		mAnimation = nullptr;
	}
}

void AnimationComponent::OnStart()
{
	mController->Play(mAnimation->GetUID(), true);
}

void AnimationComponent::OnUpdate()
{
	mController->Update(mOwner->GetChildren()[0]);
}

void AnimationComponent::SetAnimation(unsigned int uid)
{
	ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(uid, Resource::Type::Animation));
	if (tmpAnimation && mAnimation)
	{
		App->GetResource()->ReleaseResource(mAnimation->GetUID());
	}
	if (tmpAnimation)
	{
		mAnimation = tmpAnimation;

		mController = new AnimationController(mAnimation, uid, true);
	}
}

void AnimationComponent::OnStop()
{
	//mController->Stop();
}

void AnimationComponent::Update()
{

}

Component* AnimationComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void AnimationComponent::Save(Archive& archive) const
{
	
}

void AnimationComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{

}