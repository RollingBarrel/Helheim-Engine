#include "AnimationComponent.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ResourceAnimation.h"

#include "AnimationController.h"
#include "AnimationStateMachine.h"
#include "AnimationComponent.h"

#include "GameObject.h"

#include "ResourceModel.h"
#include "float4x4.h"

AnimationComponent::AnimationComponent(GameObject* owner) : Component(owner, ComponentType::ANIMATION), mAnimationUid(0), mController(nullptr), mModelUid(0)
{
	mStateMachine = nullptr;
	mSpeed = 1.0;
}

AnimationComponent::AnimationComponent(const AnimationComponent& other, GameObject* owner) : Component(owner, ComponentType::ANIMATION)
{

	mSpeed = 1.0;
	mStateMachine = nullptr;

	mModelUid = other.mModelUid;

	SetAnimation(other.mAnimationUid);


}

AnimationComponent::~AnimationComponent()
{
	delete mController;
	delete mStateMachine;
	mGameobjectsInverseMatrices.clear();
	mPalette.clear();
	
}

void AnimationComponent::SetLoop(bool loop)
{
	mLoop = loop;
	mController->SetLoop(loop);
}

void AnimationComponent::OnStart()
{
	if (mGameobjectsInverseMatrices.size() == 0 && mModelUid != 0 && mAnimationUid != 0)
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
		mAnimationUid = uid;
		if (mController)
		{
			delete mController;
		}
		mController = new AnimationController(tmpAnimation, uid, true);
	}
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
				mAnimationUid = tmpAnimation->GetUID();
				mController = new AnimationController(tmpAnimation, mAnimationUid, true);
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
	ChangeState("default", 0.0);

	
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
	archive.AddInt("AnimationID", mAnimationUid);
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

	int modelUid = { 0 };

	if (data.HasMember("ModelUID") && data["ModelUID"].IsInt()) 
	{
		modelUid = data["ModelUID"].GetInt();
	}

	SetModelUUID(modelUid);
}