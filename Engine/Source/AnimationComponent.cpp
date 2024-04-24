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

void AnimationComponent::SetLoop(bool loop)
{
	mLoop = loop;
	mController->SetLoop(loop);
}

void AnimationComponent::OnStart()
{
	//mController->Play(mAnimation->GetUID(), true);
}

void AnimationComponent::Update()
{
	if (mIsPlaying)
		mController->Update(mOwner);
	//else {
	//
	//}
}

void AnimationComponent::OnStop()
{

}

void AnimationComponent::OnRestart()
{
	mController->Restart();
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

		if (mController)
			delete mController;
		mController = new AnimationController(mAnimation, uid, true);
	}
}

void AnimationComponent::SetStartTime(float time)
{
	mController->SetStartTime(time);
}

void AnimationComponent::SetEndTime(float time)
{
	mController->SetEndTime(time);
}

Component* AnimationComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void AnimationComponent::Save(Archive& archive) const
{
	archive.AddInt("ID", GetID());
	archive.AddInt("AnimationID", mAnimation->GetUID());
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());

}

void AnimationComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{

	int animationID = { 0 };
	if (data.HasMember("AnimationID") && data["AnimationID"].IsInt()) {
		animationID = data["AnimationID"].GetInt();
	}
	SetAnimation(animationID);
}