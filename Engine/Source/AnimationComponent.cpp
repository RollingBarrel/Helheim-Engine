#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"

#include "AnimationComponent.h"

#include "GameObject.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION), mAnimation(nullptr), mController(nullptr)
{
	mClipNames.clear();
	mClipTimes.clear();

	mClipNames.reserve(3);
	mClipTimes.reserve(6);

	mClipNames.push_back("Walk");
	mClipTimes.push_back(0.0);
	mClipTimes.push_back(1.25);

	mClipNames.push_back("Idle");
	mClipTimes.push_back(1.25);
	mClipTimes.push_back(12.0);

	mClipNames.push_back("Die");
	mClipTimes.push_back(12.0);
	mClipTimes.push_back(15.0);

	mCurrentClip = 0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION)
{
	mAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(other.mAnimation->GetUID(), Resource::Type::Mesh));

	mController = other.mController;

	mClipNames.clear();
	mClipTimes.clear();

	mClipNames.reserve(3);
	mClipTimes.reserve(6);

	mClipNames.push_back("Walk");
	mClipTimes.push_back(0.0);
	mClipTimes.push_back(1.25);

	mClipNames.push_back("Idle");
	mClipTimes.push_back(1.25);
	mClipTimes.push_back(12.0);

	mClipNames.push_back("Die");
	mClipTimes.push_back(12.0);
	mClipTimes.push_back(15.0);

	mCurrentClip = 0;
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
	if (mGameobjectsInverseMatrices.size() == 0)
	{
		//LoadAllChildJoints(mOwner);
		for (int i = 0; i < mAnimation->mInvBindMatrices.size(); i++) 
		{
			mGameobjectsInverseMatrices.push_back(std::pair(mOwner->FindGameObjectInTree(mAnimation->mInvBindMatrices[i].first), mAnimation->mInvBindMatrices[i].second));
		}
	}
	//mController->Play(mAnimation->GetUID(), true);
}

void AnimationComponent::Update()
{
	if (mIsPlaying)
	{
		mController->Update(mOwner);
		UpdatePalette();
	}
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
	mClipTimes[mCurrentClip * 2] = time;
	mController->SetStartTime(time);
}

void AnimationComponent::SetEndTime(float time)
{
	mClipTimes[mCurrentClip * 2+1] = time;
	mController->SetEndTime(time);
}


//void AnimationComponent::AddJointNode(GameObject* node)
//{
//	ResourceAnimation::AnimationChannel* animChannel = mAnimation->GetChannel(node->GetName());
//	if (animChannel /*&& !animChannel->invBindMatrix.Equals(float4x4::identity)*/)
//	{
//		mGameobjectsInverseMatrices.push_back(std::pair(node, animChannel->invBindMatrix));
//	}
//}

//void AnimationComponent::LoadAllChildJoints(GameObject* currentObject)
//{
//	AddJointNode(currentObject);
//	for (const auto& object : currentObject->GetChildren())
//	{
//		LoadAllChildJoints(object);
//	}
//
//}

void AnimationComponent::UpdatePalette()
{
	mPalette.clear();
	mPalette.reserve(mGameobjectsInverseMatrices.size());
	for (unsigned i = 0; i < mGameobjectsInverseMatrices.size(); ++i)
	{
		mPalette.push_back((mGameobjectsInverseMatrices[i].first->TranformInFirstGameObjectSpace() * mGameobjectsInverseMatrices[i].second).Transposed());
	}

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