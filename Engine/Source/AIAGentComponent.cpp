#include "AIAgentComponent.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"
#include "DetourCrowd.h"
#include "DetourCommon.h"
#include "MathConstants.h"
#include "Physics.h"
#include "Geometry/Ray.h"
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
	if (App->IsPause()) return;
	if (mNavPositions.size() > 0 && App->IsPlayMode())
	{
		if (mCrowdId != CROWD_OFF_INDEX)
		{
			float3 owner_pos = mOwner->GetWorldPosition();
			App->GetNavigation()->MoveAgent(mCrowdId, owner_pos);
			mDirection = owner_pos - mOwner->GetWorldPosition();
			mOwner->SetWorldPosition(owner_pos);

			/*
			float3 destination = mNavPositions.size() > 1 ? mNavPositions[1] : mNavPositions[0];
			if (owner_pos.Equals(destination, 0.5f) && mNavPositions.size() > 2)
			{
				SetNavigationPath(mNavPositions.back());
			}
			*/
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
	if (!mNavPositions.empty())
	{
		App->GetNavigation()->SetAgentDestination(mCrowdId, mNavPositions.back());
		/*
		if (mNavPositions.size() > 1)
		{
			App->GetNavigation()->SetAgentDestination(mCrowdId, mNavPositions[1]);
		}
		else
		{
			App->GetNavigation()->SetAgentDestination(mCrowdId, mNavPositions[0]);
		}
		*/
	}	
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
		agentParams.collisionQueryRange = agentParams.radius * 3.0f;
		agentParams.pathOptimizationRange = agentParams.radius * 30.0f;
		agentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE | DT_CROWD_OPTIMIZE_VIS | DT_CROWD_OPTIMIZE_TOPO | DT_CROWD_SEPARATION;
		agentParams.obstacleAvoidanceType = (unsigned char)3.0f;
		agentParams.separationWeight = 14.0f;

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

bool AIAgentComponent::FleeFromTarget(const float3& target)
{
	float3 ownerPos = mOwner->GetWorldPosition();
	float3 fleeDirection = ownerPos - target;
	fleeDirection.Normalize();
	float fleeDistance = 5.0f;
	
	float3 fleeTarget = float3::zero;
	dtVmad(&fleeTarget[0], &ownerPos[0], &fleeDirection[0], fleeDistance);

	dtPolyRef targetPoly = 0;
	float3 nearestPoint = float3::zero;
	dtQueryFilter queryFilter;
	float3 queryAreaSize = float3(5.0f);

	const int numDirections = 24;
	float3 bestFleeTarget = float3::zero;
	dtPolyRef bestTargetPoly = 0;
	float3 bestNearestPoint = float3::zero;
	float bestDistance = 0.0f;

	float3 rayOrigin = mOwner->GetWorldPosition();
	rayOrigin.y += 1.70f;
	Hit hit;
	Ray ray;
	ray.dir = fleeDirection;
	ray.pos = rayOrigin;
	Physics::Raycast(hit, ray, 30.0f);
	if (hit.IsValid())
	{
		if (hit.mGameObject->GetTag().compare("Door") == 0)
		{
			for (int i = 0; i < numDirections; ++i)
			{
				float angle = (i / (float)numDirections) * (2 * pi);
				float3 rotatedDirection = float3::zero;
				rotatedDirection[0] = fleeDirection[0] * cos(angle) - fleeDirection[2] * sin(angle);
				rotatedDirection[1] = fleeDirection[1];
				rotatedDirection[2] = fleeDirection[0] * sin(angle) + fleeDirection[2] * cos(angle);
				dtVmad(&fleeTarget[0], &ownerPos[0], &rotatedDirection[0], fleeDistance);
				ray.dir = rotatedDirection;
				Physics::Raycast(hit, ray, fleeDistance);
				if (hit.IsValid())
				{
					if (hit.mGameObject->GetTag().compare("Door") == 0)
					{
						continue;
					}
				}
				App->GetNavigation()->GetCrowd()->getNavMeshQuery()->findNearestPoly(&fleeTarget[0], &queryAreaSize[0], &queryFilter, &targetPoly, &nearestPoint[0]);
				if (targetPoly)
				{
					float distanceToFleePoint = dtVdist(&ownerPos[0], &nearestPoint[0]);
					if (bestTargetPoly == 0 || distanceToFleePoint > bestDistance)
					{
						bestTargetPoly = targetPoly;
						bestNearestPoint = nearestPoint;
						bestDistance = distanceToFleePoint;
					}
				}
			}
			if (bestTargetPoly)
			{
				App->GetNavigation()->GetCrowd()->requestMoveTarget(mCrowdId, bestTargetPoly, &bestNearestPoint[0]);
				return true;
			}
			return false;
		}

	}
	
	

	App->GetNavigation()->GetCrowd()->getNavMeshQuery()->findNearestPoly(&fleeTarget[0], &queryAreaSize[0], &queryFilter, &targetPoly, &nearestPoint[0]);

	if (targetPoly)
	{
		App->GetNavigation()->GetCrowd()->requestMoveTarget(mCrowdId, targetPoly, &nearestPoint[0]);
		return true;
	}
	else
	{
		bestFleeTarget = float3::zero;
		bestTargetPoly = 0;
		bestNearestPoint = float3::zero;
		bestDistance = 0.0f;
		for (int i = 0; i < numDirections; ++i)
		{
			float angle = (i / (float)numDirections) * (2 * pi);
			float3 rotatedDirection = float3::zero;
			rotatedDirection[0] = fleeDirection[0] * cos(angle) - fleeDirection[2] * sin(angle);
			rotatedDirection[1] = fleeDirection[1];
			rotatedDirection[2] = fleeDirection[0] * sin(angle) + fleeDirection[2] * cos(angle);
			dtVmad(&fleeTarget[0], &ownerPos[0], &rotatedDirection[0], fleeDistance);
			ray.dir = rotatedDirection;
			Physics::Raycast(hit, ray, fleeDistance);
			if (hit.IsValid())
			{
				if (hit.mGameObject->GetTag().compare("Door") == 0)
				{
					continue;
				}
			}
			App->GetNavigation()->GetCrowd()->getNavMeshQuery()->findNearestPoly(&fleeTarget[0], &queryAreaSize[0], &queryFilter, &targetPoly, &nearestPoint[0]);
			if (targetPoly)
			{
				float distanceToFleePoint = dtVdist(&ownerPos[0], &nearestPoint[0]);
				if (bestTargetPoly == 0 || distanceToFleePoint > bestDistance)
				{
					bestTargetPoly = targetPoly;
					bestNearestPoint = nearestPoint;
					bestDistance = distanceToFleePoint;
				}
			}
		}
		if (bestTargetPoly)
		{
			App->GetNavigation()->GetCrowd()->requestMoveTarget(mCrowdId, bestTargetPoly, &bestNearestPoint[0]);
			return true;
		}
		return false;
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