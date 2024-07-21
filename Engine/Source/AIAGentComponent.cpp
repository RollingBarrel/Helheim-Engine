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

	mRadius = original.mRadius;
	mHeight = original.mHeight;
	mMaxAcceleration = original.mMaxAcceleration;
	mMaxSpeed = original.mMaxSpeed;
}

void AIAgentComponent::Reset()
{
}


AIAgentComponent::~AIAgentComponent()
{
	PauseCrowdNavigation();
}

void AIAgentComponent::Update()
{
	if (mNavPositions.size() > 0 && App->IsPlayMode())
	{
		if (mCrowdId != CROWD_OFF_INDEX)
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
	App->GetNavigation()->SetAgentDestination(mCrowdId, mNavPositions.back());
}

void AIAgentComponent::StartCrowdNavigation()
{
	if (mCrowdId == CROWD_OFF_INDEX)
	{
		dtCrowdAgentParams agentParams;
		memset(&agentParams, 0, sizeof(agentParams));

		//AI Agent Class params
		agentParams.radius = mRadius;
		agentParams.height = mHeight;
		agentParams.maxAcceleration = mMaxAcceleration;
		agentParams.maxSpeed = mMaxSpeed;

		//Crowd fixed params
		agentParams.collisionQueryRange = agentParams.radius * 12.0f;
		agentParams.pathOptimizationRange = agentParams.radius * 30.0f;
		agentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_SEPARATION;
		agentParams.obstacleAvoidanceType = (unsigned char)3.0f;
		agentParams.separationWeight = 2.0f;

		mCrowdId = App->GetNavigation()->AddAgent(mOwner->GetWorldPosition(), agentParams);
	}
	else
	{
		PauseCrowdNavigation();
		StartCrowdNavigation();
	}

}

void AIAgentComponent::PauseCrowdNavigation()
{
	if (mCrowdId != CROWD_OFF_INDEX)
	{
		if (App->GetNavigation())
		{
			App->GetNavigation()->DisableAgent(mCrowdId);

		}
		mCrowdId = CROWD_OFF_INDEX;
		
	}
}


void AIAgentComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	obj.AddFloat("Height", mHeight);
	obj.AddFloat("Radius", mRadius);
	obj.AddFloat("MaxSpeed", mMaxSpeed);
	obj.AddFloat("MaxAcceleration", mMaxAcceleration);

}

void AIAgentComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	if (data.HasMember("Height"))
	{
		mHeight = data.GetFloat("Height");
	}
	if (data.HasMember("Radius"))
	{
		mRadius = data.GetFloat("Radius");
	}
	if (data.HasMember("MaxSpeed"))
	{
		mMaxSpeed = data.GetFloat("MaxSpeed");
	}
	if (data.HasMember("MaxAcceleration"))
	{
		mMaxAcceleration = data.GetFloat("MaxAcceleration");
	}
}

void AIAgentComponent::Enable()
{
	StartCrowdNavigation();	
}

void AIAgentComponent::Disable()
{
	if (mCrowdId != CROWD_OFF_INDEX)
	{
		PauseCrowdNavigation();
	}
}