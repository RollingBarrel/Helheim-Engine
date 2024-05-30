#include "AnimationComponent.h"
#include "AnimationComponent.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"
#include "AnimationStateMachine.h"

#include "GameObject.h"

#include "ResourceModel.h"
#include "float4x4.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION),  mController(nullptr), mModelUid(0), mSpineController(nullptr), mSpineGameObject(nullptr), mSpineStateMachine(nullptr)
{
	mStateMachine = nullptr;
	mSpeed = 1.0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION), mController(nullptr)
{

	mSpeed = 1.0;
	mStateMachine = nullptr;

	SetModelUUID(other.mModelUid);

}

AnimationComponent::~AnimationComponent()
{
	delete mController;
	delete mStateMachine;

	delete mSpineController;
	delete mSpineStateMachine;
	
}

void AnimationComponent::SetLoop(bool loop)
{
	mLoop = loop;
	mController->SetLoop(loop);
}

void AnimationComponent::OnStart()
{
	
}

void AnimationComponent::Update()
{
	if (mIsPlaying)
	{
		if (mController)
		{
			mController->Update(mOwner);
		}
	}
}

void AnimationComponent::OnStop()
{

}

void AnimationComponent::OnRestart()
{
	mController->Restart();
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

void AnimationComponent::SendTrigger(std::string trigger, float transitionTime)
{

	std::string currentStateName = GetCurrentStateName();

	for (size_t i = 0; i < mStateMachine->GetnNumTransitions(); i++)
	{
		if (currentStateName == mStateMachine->GetTransitionSource(i) && trigger == mStateMachine->GetTransitionTrigger(i))
		{
			ChangeState(mStateMachine->GeTransitionTarget(i), transitionTime);
		}
	}

}

void AnimationComponent::ChangeState(std::string stateName, float transitionTime)
{
	int stateIndex = mStateMachine->GetStateIndex(stateName);

	if (stateIndex < mStateMachine->GetNumStates() /*&& (stateIndex == 0 || stateIndex != mCurrentState)*/)
	{
		mCurrentState = stateIndex;
		
		int clipIndex = mStateMachine->GetClipIndex(mStateMachine->GetStateClip(stateIndex));

		unsigned int resourceAnimation = mStateMachine->GetClipResource(clipIndex);

		if (resourceAnimation !=0)
		{
			ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
			if (!tmpAnimation)
			{
				LOG("ERROR loading animation on new state.");
			}
			if (mController)
			{
				mController->SetNextAnimation(tmpAnimation);
				float new_clip_start = mStateMachine->GetStateStartTime(stateIndex);
				float new_clip_end = mStateMachine->GetStateEndTime(stateIndex);
				mController->SetClipStartTime(new_clip_start);
				mController->SetStartTime(new_clip_start);
				mController->SetEndTime(new_clip_end);
				mController->SetTransitionDuration(transitionTime);
				mController->ActivateTransition();

			}
			else
			{
				if (mController)
				{
					delete mController;
				}
				mController = new AnimationController(tmpAnimation, true);
				mController->SetStartTime(mStateMachine->GetStateStartTime(stateIndex));
				mController->SetEndTime(mStateMachine->GetStateEndTime(stateIndex));

			}
			
		}
	}

}

void AnimationComponent::SetModelUUID(unsigned int modelUid)
{
	if (modelUid == mModelUid)
		return;
	mModelUid = modelUid;
	ResourceModel* my_model = reinterpret_cast<ResourceModel*>(App->GetResource()->RequestResource(modelUid, Resource::Type::Model));
	if(mStateMachine)
		delete mStateMachine;
	mStateMachine = new AnimationStateMachine(my_model->mAnimationUids);
	ChangeState("default", 0.0f);
	App->GetResource()->ReleaseResource(mModelUid);

	
}

void AnimationComponent::SetModel(ResourceModel* model)
{
	mModelUid = model->GetUID();
	if (mStateMachine)
		delete mStateMachine;
	mStateMachine = new AnimationStateMachine(model->mAnimationUids);
	ChangeState("default", 0.0f);


}

void AnimationComponent::StartTransition(float transitionDuration)
{
	mController->ActivateTransition();
	mController->SetTransitionDuration(transitionDuration);
}

Component* AnimationComponent::Clone(GameObject* owner) const
{
	return new AnimationComponent(*this, owner);
}

void AnimationComponent::Save(Archive& archive) const
{
	archive.AddInt("ID", GetID());
	archive.AddInt("ModelUID", mModelUid);

	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());

}

void AnimationComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	int modelUid = { 0 };

	if (data.HasMember("ModelUID") && data["ModelUID"].IsInt()) 
	{
		modelUid = data["ModelUID"].GetInt();
	}

	SetModelUUID(modelUid);
}