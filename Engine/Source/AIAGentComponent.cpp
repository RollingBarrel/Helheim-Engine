#include "AIAgentComponent.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
#include "DetourCrowd.h"

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
	if (mNavPositions.size() > 0)
	{
		/*
		if (mOwner->GetWorldPosition().Equals(mNavPositions[0], 0.1f))
		{
			mMoving = false;
		}
		*/
		if (mMoving && mCrowdId != 101)
		{
			float3 owner_pos = mOwner->GetWorldPosition();
			App->GetNavigation()->MoveAgent(mCrowdId, owner_pos);
			mOwner->SetWorldPosition(owner_pos);
		}

	}
}

Component* AIAgentComponent::Clone(GameObject* owner) const
{
	return new AIAgentComponent(*this, owner);
}
void AIAgentComponent::SetNavigationPath(const float3& destination) 
{
	mNavPositions = App->GetNavigation()->FindNavPath(GetOwner()->GetWorldPosition(), destination);
	mMoving = true;
	App->GetNavigation()->SetAgentDestination(mCrowdId, mNavPositions.back());
}

void AIAgentComponent::StartCrowdNavigation()
{
	if (mCrowdId == 101)
	{
		dtCrowdAgentParams agentParams;
		memset(&agentParams, 0, sizeof(agentParams));

		//AI Agent Class params
		agentParams.radius = 0.6f;
		agentParams.height = 2.0f;
		agentParams.maxAcceleration = 16.0f;
		agentParams.maxSpeed = 5.0f;

		//Crowd fixed params
		agentParams.collisionQueryRange = agentParams.radius * 12.0f;
		agentParams.pathOptimizationRange = agentParams.radius * 30.0f;
		agentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_SEPARATION;
		agentParams.obstacleAvoidanceType = (unsigned char)3.0f;
		agentParams.separationWeight = 2.0f;

		mCrowdId = App->GetNavigation()->AddAgent(mOwner->GetWorldPosition(), agentParams);

	}
}

void AIAgentComponent::PauseCrowdNavigation()
{
	if (mCrowdId != 101)
	{
		App->GetNavigation()->DisableAgent(mCrowdId);
		mCrowdId = 101;
	}
}

void AIAgentComponent::MoveAgent(float speed ) const
{
	/*
	if (mNavPositions.size() > 1 )
	{
		//LOG("START")
		for(float3 nav : mNavPositions)
		{
			//LOG("%f, %f, %f", nav.x, nav.y, nav.z);
		}
		//LOG("END")
			float3 direction = (mNavPositions[1] - mNavPositions[0]).Normalized();
			direction = direction / 50 * speed;
			GetOwner()->SetWorldPosition(GetOwner()->GetWorldPosition() + direction);
	}
	*/
	
}


void AIAgentComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	//obj.AddFloat("Radius", mRadius);
	//obj.AddFloat("Height", mHeight);
	//obj.AddFloat("StepHeight", mStepHeight);
	//obj.AddInt("MaxSlope", mMaxSlope);
	//obj.AddFloat("Speed", mSpeed);
	//obj.AddFloat("AngularSpeed", mAngularSpeed);
	//obj.AddFloat("Acceleration", mAcceleration);
	//obj.AddFloat("StoppingDistance", mStoppingDistance);
}

void AIAgentComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
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

