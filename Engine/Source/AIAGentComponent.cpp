#include "AIAgentComponent.h"

AIAgentComponent::AIAgentComponent(GameObject* ownerGameObject)
 : Component(ownerGameObject, ComponentType::AIAGENT)
{

}

AIAgentComponent::AIAgentComponent(const AIAgentComponent& original, GameObject* owner)
	: Component(owner, ComponentType::AIAGENT)
{
	mRadius = original.mRadius;
	 mHeight = original.mHeight;
	 mStepHeight = original.mStepHeight;
	 mMaxSlope = original.mMaxSlope;
	 mSpeed = original.mSpeed;
	 mAngularSpeed = original.mAngularSpeed;
	 mAcceleration = original.mAcceleration;
	 mStoppingDistance = original.mStoppingDistance;
}

void AIAgentComponent::Reset()
{
	//Agent Parameters:
	 mRadius = 0.0f;
	 mHeight = 0.0f;
	 mStepHeight = 0.0f;
	 mMaxSlope = 0;
	//Steering Parameters:
	 mSpeed = 0.0f;
	 mAngularSpeed = 0.0f; 
	 mAcceleration = 0.0f;
	 mStoppingDistance = 0.0f; 
}

AIAgentComponent::~AIAgentComponent()
{
}

void AIAgentComponent::Update()
{
}

Component* AIAgentComponent::Clone(GameObject* owner) const
{
	return new AIAgentComponent(*this, owner);
}

void AIAgentComponent::Save(Archive& archive) const
{
}

void AIAgentComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}

