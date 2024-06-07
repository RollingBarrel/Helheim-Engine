#include "AIAgentComponent.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
AIAgentComponent::AIAgentComponent(GameObject* ownerGameObject)
 : Component(ownerGameObject, ComponentType::AIAGENT)
{

}

AIAgentComponent::AIAgentComponent(const AIAgentComponent& original, GameObject* owner)
	: Component(owner, ComponentType::AIAGENT)
{
	//mRadius = original.mRadius;
	// mHeight = original.mHeight;
	// mStepHeight = original.mStepHeight;
	// mMaxSlope = original.mMaxSlope;
	 //mSpeed = original.mSpeed;
	// mAngularSpeed = original.mAngularSpeed;
	// mAcceleration = original.mAcceleration;
	// mStoppingDistance = original.mStoppingDistance;
}

void AIAgentComponent::Reset()
{
	////Agent Parameters:
	// mRadius = 0.0f;
	// mHeight = 0.0f;
	// mStepHeight = 0.0f;
	// mMaxSlope = 0;
	////Steering Parameters:
	 //mSpeed = 0.0f;
	// mAngularSpeed = 0.0f; 
	// mAcceleration = 0.0f;
	// mStoppingDistance = 0.0f; 
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
void AIAgentComponent::SetNavigationPath(const float3& destination) 
{
	mNavPositions = App->GetNavigation()->FindNavPath(GetOwner()->GetPosition(), destination);
}

void AIAgentComponent::MoveAgent(float speed ) const
{

	if (mNavPositions.size() > 1 )
	{
		LOG("START")
		for(float3 nav : mNavPositions)
		{
			LOG("%f, %f, %f", nav.x, nav.y, nav.z);
		}
		LOG("END")
			float3 direction = (mNavPositions[1] - mNavPositions[0]).Normalized();
			direction = direction / 50 * speed;
			GetOwner()->SetPosition(GetOwner()->GetPosition() + direction);
	}
	
}


void AIAgentComponent::Save(Archive& archive) const
{
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	//archive.AddFloat("Radius", mRadius);
	//archive.AddFloat("Height", mHeight);
	//archive.AddFloat("StepHeight", mStepHeight);
	//archive.AddInt("MaxSlope", mMaxSlope);
	//archive.AddFloat("Speed", mSpeed);
	//archive.AddFloat("AngularSpeed", mAngularSpeed);
	//archive.AddFloat("Acceleration", mAcceleration);
	//archive.AddFloat("StoppingDistance", mStoppingDistance);
}

void AIAgentComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	//if (data.HasMember("Radius")) 
	//{
	//	mRadius = data["Radius"].GetFloat();
	//}
	//if (data.HasMember("Height")) 
	//{
	//	mHeight = data["Height"].GetFloat();
	//}
	//if (data.HasMember("StepHeight")) 
	//{
	//	mStepHeight = data["StepHeight"].GetFloat();
	//}
	//if (data.HasMember("MaxSlope")) 
	//{
	//	mMaxSlope = data["MaxSlope"].GetInt();
	//}
	//if (data.HasMember("Speed")) 
	//{
	//	mSpeed = data["Speed"].GetFloat();
	//}
	//if (data.HasMember("AngularSpeed")) 
	//{
	//	mRadius = data["AngularSpeed"].GetFloat();
	//}
	//if (data.HasMember("Acceleration")) 
	//{
	//	mRadius = data["Acceleration"].GetFloat();
	//}
	//if (data.HasMember("StoppingDistance")) 
	//{
	//	mRadius = data["StoppingDistance"].GetFloat();
	//}
}

