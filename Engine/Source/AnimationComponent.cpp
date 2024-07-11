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
	mAnimationUID = 0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION), mController(nullptr), mSpineController(nullptr), mSpineStateMachine(nullptr)
{

	mSpeed = 1.0;
	mStateMachine = nullptr;
	mAnimationUID = other.mAnimationUID;

	ResourceAnimation* res = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(other.mAnimationUID, Resource::Type::Animation));
	res->AddReferenceCount();

	mController = new AnimationController(res);
	mSpineController = new AnimationController(res);

	mStateMachine = new AnimationStateMachine(other.mAnimationUID);
	mSpineStateMachine = new AnimationStateMachine(other.mAnimationUID);

}

AnimationComponent::~AnimationComponent()
{
	// Destructor calls all his members destrcuctors?
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
	
	mStateMachine = new AnimationStateMachine(mAnimationUID);
	mSpineStateMachine = new AnimationStateMachine(mAnimationUID);
	
	ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(mAnimationUID, Resource::Type::Animation));
	mController = new AnimationController(tmpAnimation);
	mController->SetStartTime(mStateMachine->GetStateStartTime(0));
	mController->SetEndTime(mStateMachine->GetStateEndTime(0));
	mStateMachine = new AnimationStateMachine(mAnimationUID);

	mSpineController = new AnimationController(tmpAnimation);
	mSpineController->SetStartTime(mStateMachine->GetStateStartTime(0));
	mSpineController->SetEndTime(mStateMachine->GetStateEndTime(0));
	mSpineStateMachine = new AnimationStateMachine(mAnimationUID);

}

void AnimationComponent::Update()
{

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

void AnimationComponent::OnReset()
{
	if (mDefaultObjects.size() == 0)
	{
		LoadGameObjects(mOwner);
	}
	ChangeState("Default", 0.0f);
	if (mHasSpine)
	{
		ChangeSpineState("Default", 0.0f);
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
	if (mDefaultObjects.size() == 0)
	{
		LoadGameObjects(mOwner);
	}
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
		

		unsigned int resourceAnimation = mStateMachine->GetUID();

		if (resourceAnimation !=0)
		{
			
			if (mController)
			{
				float new_clip_start = mStateMachine->GetStateStartTime(stateIndex);
				float new_clip_end = mStateMachine->GetStateEndTime(stateIndex);
				bool is_looped = mStateMachine->GetStateLoop(stateIndex);
				mController->SetLoop(is_looped);
				mController->SetClipStartTime(new_clip_start);
				mController->SetStartTime(new_clip_start);
				mController->SetEndTime(new_clip_end);
				mController->SetTransitionDuration(transitionTime);
				mController->ActivateTransition();

			}
			else
			{
				ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
				mController = new AnimationController(tmpAnimation);
				mController->SetLoop(mStateMachine->GetStateLoop(stateIndex));
				mController->SetStartTime(mStateMachine->GetStateStartTime(stateIndex));
				mController->SetEndTime(mStateMachine->GetStateEndTime(stateIndex));

			}
			
		}
	}

}


bool AnimationComponent::HasCurrentStateFinished(float deltaTime)
{
	if (mController->GetLoop())
	{
		return false;
	}
	float timeDiff = mController->GetAnimationCurrentTime() - mController->GetEndTime();
	timeDiff = timeDiff > 0 ? timeDiff : -timeDiff;
	return timeDiff<deltaTime;
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
	if (mDefaultObjects.size() == 0)
	{
		LoadGameObjects(mOwner);
	}
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


		unsigned int resourceAnimation = mSpineStateMachine->GetUID();

		if (resourceAnimation != 0)
		{
			if (mSpineController)
			{
				float new_clip_start = mSpineStateMachine->GetStateStartTime(stateIndex);
				float new_clip_end = mSpineStateMachine->GetStateEndTime(stateIndex);
				bool is_looped = mSpineStateMachine->GetStateLoop(stateIndex);
				mSpineController->SetLoop(is_looped);
				mSpineController->SetClipStartTime(new_clip_start);
				mSpineController->SetStartTime(new_clip_start);
				mSpineController->SetEndTime(new_clip_end);
				mSpineController->SetTransitionDuration(transitionTime);
				mSpineController->ActivateTransition();

			}
			else
			{
				ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(resourceAnimation, Resource::Type::Animation));
				mSpineController = new AnimationController(tmpAnimation);
				mSpineController->SetLoop(mSpineStateMachine->GetStateLoop(stateIndex));
				mSpineController->SetStartTime(mSpineStateMachine->GetStateStartTime(stateIndex));
				mSpineController->SetEndTime(mSpineStateMachine->GetStateEndTime(stateIndex));

			}

		}
	}
}


bool AnimationComponent::HasCurrentSpineStateFinished(float deltaTime)
{
	if (!mHasSpine)
	{
		return false;
	}
	if (mSpineController->GetLoop())
	{
		return false;
	}
	float timeDiff = mSpineController->GetAnimationCurrentTime() - mSpineController->GetEndTime();
	timeDiff = timeDiff > 0 ? timeDiff : -timeDiff;
	return timeDiff < deltaTime;
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

void AnimationComponent::ReloadGameObjects()
{
	mHasSpine = false;
	mDefaultObjects.clear();
	mSpineObjects.clear();
	LoadGameObjects(mOwner);
}



void AnimationComponent::LoadGameObjects(GameObject* current)
{
	if (current->GetName() == std::string("Spine") || current->GetName() == std::string("mixamorig:Spine"))
	{
		if (mOwner->GetTag() == "Player")
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

	obj.AddInt("AnimationUID", mAnimationUID);
	obj.AddInt("LowerSMUID", mStateMachine->GetUID());

	if (mHasSpine)
	{
		obj.AddInt("UpperSMUID", mSpineStateMachine->GetUID());
	}
}

void AnimationComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data,uidPointerMap);

	ReloadGameObjects();

	if (data.HasMember("AnimationUID"))
	{
		mAnimationUID = data.GetInt("AnimationUID");


		ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(mAnimationUID, Resource::Type::Animation));
		mController = new AnimationController(tmpAnimation);
		mSpineController = new AnimationController(tmpAnimation);


		if (data.HasMember("LowerSMUID"))
		{
			int lowerStateMachine = data.GetInt("LowerSMUID");
			if (lowerStateMachine == 0)
			{
				mStateMachine = new AnimationStateMachine(mAnimationUID);
			}
			else
			{
				ResourceStateMachine* resSM = reinterpret_cast<ResourceStateMachine*>(App->GetResource()->RequestResource(lowerStateMachine, Resource::Type::StateMachine));
				mStateMachine = resSM->GetStateMachine();
				mStateMachine->SetUID(resSM->GetUID());
				App->GetResource()->ReleaseResource(resSM->GetUID());

			}
		}
		else
		{
			mStateMachine = new AnimationStateMachine(mAnimationUID);
		}
		

		mController->SetStartTime(mStateMachine->GetStateStartTime(0));
		mController->SetEndTime(mStateMachine->GetStateEndTime(0));

		if (data.HasMember("UpperSMUID"))
		{
			int upperStateMachine = data.GetInt("UpperSMUID");
			if (upperStateMachine == 0)
			{
				mSpineStateMachine = new AnimationStateMachine(mAnimationUID);
			}
			else
			{
				ResourceStateMachine* resSM = reinterpret_cast<ResourceStateMachine*>(App->GetResource()->RequestResource(upperStateMachine, Resource::Type::StateMachine));
				mSpineStateMachine = resSM->GetStateMachine();
				mSpineStateMachine->SetUID(resSM->GetUID());
				mSpineController = new AnimationController(tmpAnimation);
				mController->SetStartTime(mSpineStateMachine->GetStateStartTime(0));
				mController->SetEndTime(mSpineStateMachine->GetStateEndTime(0));
				App->GetResource()->ReleaseResource(upperStateMachine);

			}

		}
		else
		{
			mSpineStateMachine = new AnimationStateMachine(mAnimationUID);
		}
	}
	// TEMPORAL TO SAVE AND LOAD ALL THE LEVELS AGAIN
	else if (data.HasMember("NumAnimationUIDs"))
	{
		int numUids = data.GetInt("NumAnimationUIDs");
		int* animationUids = new int[numUids];
		if (data.HasMember("AnimationsUIDs"))
		{
			data.GetInts("AnimationsUIDs", animationUids);

			mAnimationUID = animationUids[0];
		}
		ResourceAnimation* tmpAnimation = reinterpret_cast<ResourceAnimation*>(App->GetResource()->RequestResource(mAnimationUID, Resource::Type::Animation));
		mController = new AnimationController(tmpAnimation);
		mSpineController = new AnimationController(tmpAnimation);

		mStateMachine = new AnimationStateMachine(mAnimationUID);
		
		if (data.HasMember("UpperSMUID"))
		{

			mSpineStateMachine = new AnimationStateMachine(mAnimationUID);
		}
	}
}