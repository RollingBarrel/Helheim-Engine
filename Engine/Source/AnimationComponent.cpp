#include "AnimationComponent.h"

#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"
#include "AnimationStateMachine.h"

#include "GameObject.h"

#include "ResourceModel.h"
#include "float4x4.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION),  mController(nullptr), mModel(nullptr), mSpineController(nullptr), mSpineStateMachine(nullptr)
{
	mStateMachine = nullptr;
	mSpeed = 1.0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION), mController(nullptr), mSpineController(nullptr), mSpineStateMachine(nullptr)
{

	mSpeed = 1.0;
	mStateMachine = nullptr;

	SetModel(other.mModel);

}

AnimationComponent::~AnimationComponent()
{
	delete mController;
	delete mStateMachine;

	if (mHasSpine)
	{
		delete mSpineController;
		delete mSpineStateMachine;
	}

	mDefaultObjects.clear();
	mSpineObjects.clear();

	App->GetResource()->ReleaseResource(mModel->GetUID());
	
}

void AnimationComponent::SetLoop(bool loop)
{
	mLoop = loop;
	mController->SetLoop(loop);
	if (mHasSpine)
	{
		mSpineController->SetLoop(loop);
	}
}

void AnimationComponent::OnStart()
{
	
}

void AnimationComponent::Update()
{
	if (mDefaultObjects.size() == 0)
	{
		LoadGameObjects(mOwner);
		if (!mHasSpine)
		{
			delete mSpineController;
			delete mSpineStateMachine;
		}
	}
	if (mIsPlaying)
	{
		if (mController)
		{
			mController->Update();
			if (mController->GetIsInTransition())
			{
				for (GameObject* current : mDefaultObjects)
				{
					mController->GetTransform_Blending(current);
				}
			}
			else
			{
				for (GameObject* current : mDefaultObjects)
				{
					mController->GetTransform(current);
				}

			}
		}

		if (mHasSpine && mSpineController)
		{
			mSpineController->Update();

			if (mSpineController->GetIsInTransition())
			{
				for (GameObject* current : mSpineObjects)
				{
					mSpineController->GetTransform_Blending(current);
				}
			}
			else
			{
				for (GameObject* current : mSpineObjects)
				{
					mSpineController->GetTransform(current);
				}
			}

		}
		
	}
}

void AnimationComponent::OnStop()
{

}

void AnimationComponent::OnRestart()
{
	mController->Restart();
	if (mHasSpine)
	{
		mSpineController->Restart();
	}
}


void AnimationComponent::SetAnimSpeed(float speed)
{
	mSpeed = speed;
	mController->SetAnimSpeed(speed);
	if (mHasSpine)
	{
		mSpineController->SetAnimSpeed(speed);
	}

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
				
				mController = new AnimationController(tmpAnimation, true);
				mController->SetStartTime(mStateMachine->GetStateStartTime(stateIndex));
				mController->SetEndTime(mStateMachine->GetStateEndTime(stateIndex));

			}
			
		}
	}

}

std::string AnimationComponent::GetCurrentSpineStateName()
{
	assert(!mHasSpine);
	std::string currentStateName;

	if (mStateMachine != nullptr && mCurrentSpineState < mStateMachine->GetNumStates())
	{
		currentStateName = mStateMachine->GetStateName(mCurrentSpineState);
	}

	return currentStateName;
}

void AnimationComponent::SendSpineTrigger(std::string trigger, float transitionTime)
{
	assert(!mHasSpine);

	std::string currentStateName = GetCurrentSpineStateName();

	for (size_t i = 0; i < mSpineStateMachine->GetnNumTransitions(); i++)
	{
		if (currentStateName == mSpineStateMachine->GetTransitionSource(i) && trigger == mSpineStateMachine->GetTransitionTrigger(i))
		{
			ChangeSpineState(mSpineStateMachine->GeTransitionTarget(i), transitionTime);
		}
	}
}

void AnimationComponent::ChangeSpineState(std::string stateName, float transitionTime)
{
	assert(!mHasSpine);

	int stateIndex = mSpineStateMachine->GetStateIndex(stateName);

	if (stateIndex < mSpineStateMachine->GetNumStates() /*&& (stateIndex == 0 || stateIndex != mCurrentState)*/)
	{
		mCurrentSpineState = stateIndex;

		int clipIndex = mSpineStateMachine->GetClipIndex(mSpineStateMachine->GetStateClip(stateIndex));

		unsigned int resourceAnimation = mSpineStateMachine->GetClipResource(clipIndex);

		if (resourceAnimation != 0)
		{
			ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
			assert(tmpAnimation);
			if (mSpineController)
			{
				mSpineController->SetNextAnimation(tmpAnimation);
				float new_clip_start = mSpineStateMachine->GetStateStartTime(stateIndex);
				float new_clip_end = mSpineStateMachine->GetStateEndTime(stateIndex);
				mSpineController->SetClipStartTime(new_clip_start);
				mSpineController->SetStartTime(new_clip_start);
				mSpineController->SetEndTime(new_clip_end);
				mSpineController->SetTransitionDuration(transitionTime);
				mSpineController->ActivateTransition();

			}
			else
			{
				mSpineController = new AnimationController(tmpAnimation, true);
				mSpineController->SetStartTime(mSpineStateMachine->GetStateStartTime(stateIndex));
				mSpineController->SetEndTime(mSpineStateMachine->GetStateEndTime(stateIndex));

			}

		}
	}
}

unsigned int AnimationComponent::GetModelUUID() const
{
	return mModel->GetUID();
}

void AnimationComponent::SetModelUUID(unsigned int modelUid)
{
	mModel = reinterpret_cast<ResourceModel*>(App->GetResource()->RequestResource(modelUid, Resource::Type::Model));
	delete mStateMachine;
	mStateMachine = new AnimationStateMachine(mModel->mAnimationUids);
	ChangeState("default", 0.0f);
	LoadSpine(mModel);	
}

void AnimationComponent::SetModel(ResourceModel* model)
{
	mModel = model;
	delete mStateMachine;
	mStateMachine = new AnimationStateMachine(model->mAnimationUids);
	ChangeState("default", 0.0f);
	LoadSpine(model);
}

void AnimationComponent::StartTransition(float transitionDuration)
{
	mController->ActivateTransition();
	mController->SetTransitionDuration(transitionDuration);
}


void AnimationComponent::LoadSpine(ResourceModel* model)
{
	
	ResourceAnimation* anim = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(model->mAnimationUids[0], Resource::Type::Animation));
	mSpineController = new AnimationController(anim, true);
	mSpineStateMachine = new AnimationStateMachine(model->mAnimationUids);
	mSpineController->SetStartTime(mSpineStateMachine->GetStateStartTime(0));
	mSpineController->SetEndTime(mSpineStateMachine->GetStateEndTime(0));


	
	
}

void AnimationComponent::LoadGameObjects(GameObject* current)
{
	if (current->GetName() == std::string("Spine") || current->GetName() == std::string("mixamorig:Spine"))
	{
		mHasSpine = true;
		mSpineObjects.push_back(current);
		for (GameObject* child : current->GetChildren())
		{
			LoadSpineChildren(child);
		}
	}
	else 
	{
		mDefaultObjects.push_back(current);
		for (GameObject* child : current->GetChildren())
		{
			LoadGameObjects(child);
		}
	}
}

void AnimationComponent::LoadSpineChildren(GameObject* current)
{
	mSpineObjects.push_back(current);
	for (GameObject* child : current->GetChildren())
	{
		LoadSpineChildren(child);
	}
}

Component* AnimationComponent::Clone(GameObject* owner) const
{
	return new AnimationComponent(*this, owner);
}

void AnimationComponent::Save(Archive& archive) const
{
	archive.AddInt("ID", GetID());
	archive.AddInt("ModelUID", mModel->GetUID());

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