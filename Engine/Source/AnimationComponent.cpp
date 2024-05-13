#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"
#include "AnimationStateMachine.h"
#include "AnimationComponent.h"

#include "GameObject.h"

#include "ResourceModel.h"
#include "float4x4.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION), mAnimation(nullptr), mController(nullptr), mModelUid(0)
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
	mSpeed = 1.0;
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
	mSpeed = 1.0;

	mModelUid = other.mModelUid;
}

AnimationComponent::~AnimationComponent()
{
	delete mController;
}

void AnimationComponent::SetLoop(bool loop)
{
	mLoop = loop;
	mController->SetLoop(loop);
}

void AnimationComponent::OnStart()
{
	if (mGameobjectsInverseMatrices.size() == 0 && mModelUid != 0)
	{
		ResourceModel* model = reinterpret_cast<ResourceModel*>(App->GetResource()->RequestResource(mModelUid, Resource::Type::Model));
		LoadAllChildJoints(mOwner, model);

	}
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
	mClipTimes[mCurrentClip * 2 + 1] = time;
	mController->SetEndTime(time);
}


void AnimationComponent::SetCurrentClip(int currentClip)
{
	if (currentClip > mClipNames.size())
	{
		currentClip = 0;
	}
	if (mCurrentClip == currentClip)
	{
		return;
	}
	mCurrentClip = currentClip;
	SetStartTime(mClipTimes[mCurrentClip * 2]);
	SetEndTime(mClipTimes[mCurrentClip * 2 + 1]);

}

void AnimationComponent::SetAnimSpeed(float speed)
{
	mSpeed = speed;
	mController->SetAnimSpeed(speed);

}

std::string AnimationComponent::GetCurrentStateName()
{
	std::string currentStateName;

	if (mStateMachine != nullptr && mCurrentState < mStateMachine->GetNumStates())
	{
		currentStateName = mStateMachine->GetStateName(mCurrentState);
	}

	return currentStateName;
}

void AnimationComponent::SendTrigger(std::string trigger)
{

	std::string currentStateName = GetCurrentStateName();

	for (size_t i = 0; i < mStateMachine->GetnNumTransitions(); i++)
	{
		if (currentStateName == mStateMachine->GetTransitionSource(i) && trigger == mStateMachine->GetTransitionTrigger(i))
		{
			ChangeState(mStateMachine->GeTransitionTarget(i));
		}
	}

}

void AnimationComponent::ChangeState(std::string stateName)
{

	int stateIndex = mStateMachine->GetStateIndex(stateName);

	if (stateIndex < mStateMachine->GetNumStates())
	{
		mCurrentState = stateIndex;
		int clipIndex = mStateMachine->GetClipIndex(mStateMachine->GetStateClip(stateIndex));

		unsigned int resourceAnimation = mStateMachine->GetClipResource(clipIndex);

		if (resourceAnimation !=0)
		{
			ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
			mController->SetNextAnimation(tmpAnimation);
			mController->SetClipStartTime(mStateMachine->GetStateStartTime(stateIndex));
			mController->ActivateTransition();
			//SetAnimation(resourceAnimation);
		}
	}

}

void AnimationComponent::AddJointNode(GameObject* node, ResourceModel* model)
{
	for (const auto& pair : model->mInvBindMatrices)
	{
		if (pair.first == node->GetName())
		{
			mGameobjectsInverseMatrices.push_back(std::pair<GameObject*, float4x4>(node, pair.second));
			break;
		}
	}
}

void AnimationComponent::LoadAllChildJoints(GameObject* currentObject, ResourceModel* model)
{
	AddJointNode(currentObject, model);
	for (const auto& object : currentObject->GetChildren())
	{
		LoadAllChildJoints(object, model);
	}

}

void AnimationComponent::UpdatePalette()
{
	mPalette.clear();
	mPalette.reserve(mGameobjectsInverseMatrices.size());
	for (unsigned i = 0; i < mGameobjectsInverseMatrices.size(); ++i)
	{
		mPalette.push_back((mGameobjectsInverseMatrices[i].first->GetWorldTransform() * mGameobjectsInverseMatrices[i].second).Transposed());
	}

}

//The animation Resource is changed immediatlly in the component but
//is passed in the controller as mNextAnimation until the transition is done
void AnimationComponent::ChangeAnimation(ResourceAnimation* animation)
{
	mAnimation = animation;
	mController->SetNextAnimation(animation);
	//Current clip doesn't make sense in this context so its value is not important
	//But it has to be 0 so ImGui::Combo() doesn't crash
	mCurrentClip = 0;
	SetStartTime(0);
	SetEndTime(mAnimation->GetDuration());
}

void AnimationComponent::StartTransition(float transitionDuration)
{
	//mController->SetStartTransitionTime();
	mController->ActivateTransition();
	mController->SetTransitionDuration(transitionDuration);
	mController->SetClipStartTime(GetCurrentStartTime());
}

Component* AnimationComponent::Clone(GameObject* owner) const
{
	return new AnimationComponent(*this, owner);
}

void AnimationComponent::Save(Archive& archive) const
{
	archive.AddInt("ID", GetID());
	archive.AddInt("AnimationID", mAnimation->GetUID());
	archive.AddInt("ModelUID", mModelUid);

	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());

}

void AnimationComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{

	int animationID = { 0 };
	if (data.HasMember("AnimationID") && data["AnimationID"].IsInt()) 
	{
		animationID = data["AnimationID"].GetInt();
	}
	SetAnimation(animationID);

	if (data.HasMember("ModelUID") && data["ModelUID"].IsInt()) 
	{
		mModelUid = data["ModelUID"].GetInt();
	}
}