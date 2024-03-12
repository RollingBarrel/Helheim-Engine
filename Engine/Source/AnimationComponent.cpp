#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"

#include "AnimationComponent.h"

#include "GameObject.h"

AnimationComponent::AnimationComponent(GameObject* owner, unsigned int animationUid) :Component(owner, ComponentType::ANIMATION) 
{
	if (animationUid != 0) {
		mAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(animationUid, Resource::Type::Animation));
	}

	mController = new AnimationController(mAnimation, animationUid, true);
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
	mController->Play(0, true);
}

void AnimationComponent::OnUpdate()
{
	mController->Update(mOwner);
}

void AnimationComponent::OnStop()
{
	//mController->Stop();
}