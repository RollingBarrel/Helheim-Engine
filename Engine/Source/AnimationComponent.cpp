#include "AnimationComponent.h"

#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"
#include "AnimationStateMachine.h"

#include "GameObject.h"

#include "float4x4.h"
#include "ResourceStateMachine.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION),  mController(nullptr), mSpineController(nullptr), mSpineStateMachine(nullptr)
{
	mStateMachine = nullptr;
	mSpeed = 1.0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION), mController(nullptr), mSpineController(nullptr), mSpineStateMachine(nullptr)
{

	mSpeed = 1.0;
	mStateMachine = nullptr;

	SetAnimationsUids(other.mAnimationsUIDs);

	ResourceAnimation* res = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(other.GetAnimationUids()[0], Resource::Type::Animation));
	res->AddReferenceCount();

	mController = new AnimationController(res, true);
	mSpineController = new AnimationController(res, true);

	mStateMachine = new AnimationStateMachine(other.mAnimationsUIDs);
	mSpineStateMachine = new AnimationStateMachine(other.mAnimationsUIDs);

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

void AnimationComponent::StartUp()
{
	assert(mAnimationsUIDs.size() > 0);
	
	mStateMachine = new AnimationStateMachine(mAnimationsUIDs);
	mSpineStateMachine = new AnimationStateMachine(mAnimationsUIDs);
	
	ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(mAnimationsUIDs[0], Resource::Type::Animation));
	mController = new AnimationController(tmpAnimation, true);
	mController->SetStartTime(mStateMachine->GetStateStartTime(0));
	mController->SetEndTime(mStateMachine->GetStateEndTime(0));
	mStateMachine = new AnimationStateMachine(mAnimationsUIDs);

	mSpineController = new AnimationController(tmpAnimation, true);
	mSpineController->SetStartTime(mStateMachine->GetStateStartTime(0));
	mSpineController->SetEndTime(mStateMachine->GetStateEndTime(0));
	mSpineStateMachine = new AnimationStateMachine(mAnimationsUIDs);

}

void AnimationComponent::Update()
{
	if (mDefaultObjects.size() == 0)
	{
		LoadGameObjects(mOwner);
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
				if (!mController->GetIsInTransition())
				{
					mController->EndBlending();
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
				if (!mSpineController->GetIsInTransition())
				{
					mSpineController->EndBlending();
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

void AnimationComponent::SendTrigger(const std::string& trigger, float transitionTime)
{

	std::string currentStateName = GetCurrentStateName();
	for (size_t i = 0; i < mStateMachine->GetNumTransitions(); i++)
	{
		if (currentStateName == mStateMachine->GetTransitionSource(i) && trigger == mStateMachine->GetTransitionTrigger(i))
		{
			ChangeState(mStateMachine->GetTransitionTarget(i), transitionTime);
		}
	}

}

void AnimationComponent::ChangeState(const std::string& stateName, float transitionTime)
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
	//Changed from !mHasSpine to mHasSpine
	assert(mHasSpine);
	std::string currentStateName;

	if (mSpineStateMachine != nullptr && mCurrentSpineState < mSpineStateMachine->GetNumStates())
	{
		currentStateName = mSpineStateMachine->GetStateName(mCurrentSpineState);
	}

	return currentStateName;
}

void AnimationComponent::SendSpineTrigger(const std::string& trigger, float transitionTime)
{
	//Changed from !mHasSpine to mHasSpine
	assert(mHasSpine);

	std::string currentStateName = GetCurrentSpineStateName();

	for (size_t i = 0; i < mSpineStateMachine->GetNumTransitions(); i++)
	{
		if (currentStateName == mSpineStateMachine->GetTransitionSource(i) && trigger == mSpineStateMachine->GetTransitionTrigger(i))
		{
			ChangeSpineState(mSpineStateMachine->GetTransitionTarget(i), transitionTime);
		}
	}
}

void AnimationComponent::ChangeSpineState(const std::string& stateName, float transitionTime)
{
	assert(mHasSpine);

	int stateIndex = mSpineStateMachine->GetStateIndex(stateName);

	if (stateIndex < mSpineStateMachine->GetNumStates() /*&& (stateIndex == 0 || stateIndex != mCurrentState)*/)
	{
		mCurrentSpineState = stateIndex;

		int clipIndex = mSpineStateMachine->GetClipIndex(mSpineStateMachine->GetStateClip(stateIndex));

		unsigned int resourceAnimation = mSpineStateMachine->GetClipResource(clipIndex);

		if (resourceAnimation != 0)
		{
			// TODO: Before doing the request check if it is the same Resource (uid) as the current to avoid loading time :)
			ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
			assert(tmpAnimation);
			if (mSpineController)
			{
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



void AnimationComponent::StartTransition(float transitionDuration)
{
	mController->ActivateTransition();
	mController->SetTransitionDuration(transitionDuration);
}

void AnimationComponent::SetSpineSMUID(unsigned int uid)
{
	mSpineStateMachine->SetUID(uid);
}

void AnimationComponent::SetSMUID(unsigned int uid)
{
	mStateMachine->SetUID(uid);
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

void AnimationComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);

	obj.AddInts("AnimationsUIDs", reinterpret_cast<const int*>(reinterpret_cast<const void*>(mAnimationsUIDs.data())), mAnimationsUIDs.size());
	obj.AddInt("NumAnimationUIDs", mAnimationsUIDs.size());

	obj.AddInt("LowerSMUID", mStateMachine->GetUID());

	if (mSpineObjects.size() > 0)
	{
		obj.AddInt("UpperSMUID", mSpineStateMachine->GetUID());
	}

}

void AnimationComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data,uidPointerMap);

	if (data.HasMember("NumAnimationUIDs"))
	{
		int numUids = data.GetInt("NumAnimationUIDs");
		int* animationUids = new int[numUids];

		if (data.HasMember("AnimationsUIDs"))
		{
			data.GetInts("AnimationsUIDs", animationUids);

			for (unsigned int i = 0; i < numUids; ++i)
			{
				mAnimationsUIDs.push_back(animationUids[i]);
			}

			assert(mAnimationsUIDs.size() > 0);
			ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(mAnimationsUIDs[0], Resource::Type::Animation));
			mController = new AnimationController(tmpAnimation, true);

			if (data.HasMember("LowerSMUID"))
			{
				int lowerStateMachine = data.GetInt("LowerSMUID");
				if (lowerStateMachine != 0)
				{
					ResourceStateMachine* resSM = reinterpret_cast<ResourceStateMachine*>(App->GetResource()->RequestResource(lowerStateMachine, Resource::Type::StateMachine));
					mStateMachine = resSM->GetStateMachine();
					mStateMachine->SetUID(resSM->GetUID());
					App->GetResource()->ReleaseResource(resSM->GetUID());
				}
				else
				{
					mStateMachine = new AnimationStateMachine(mAnimationsUIDs);
				}
			}

			mController->SetStartTime(mStateMachine->GetStateStartTime(0));
			mController->SetEndTime(mStateMachine->GetStateEndTime(0));

			if (data.HasMember("UpperSMUID"))
			{
				int upperStateMachine = data.GetInt("UpperSMUID");

				if (upperStateMachine != 0)
				{
					ResourceStateMachine* resSM = reinterpret_cast<ResourceStateMachine*>(App->GetResource()->RequestResource(upperStateMachine, Resource::Type::StateMachine));
					mSpineStateMachine = resSM->GetStateMachine();
					mSpineStateMachine->SetUID(resSM->GetUID());
					mSpineController = new AnimationController(tmpAnimation, true);
					mController->SetStartTime(mSpineStateMachine->GetStateStartTime(0));
					mController->SetEndTime(mSpineStateMachine->GetStateEndTime(0));
					App->GetResource()->ReleaseResource(upperStateMachine);


				}
				else
				{
					mSpineController = new AnimationController(tmpAnimation, true);
					mSpineController->SetStartTime(mStateMachine->GetStateStartTime(0));
					mSpineController->SetEndTime(mStateMachine->GetStateEndTime(0));
					mSpineStateMachine = new AnimationStateMachine(mAnimationsUIDs);

				}
			}
		}
	}
}