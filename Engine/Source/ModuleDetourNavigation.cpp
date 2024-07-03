#include "ModuleDetourNavigation.h"
#include "Application.h"
#include "GameObject.h"
#include "NavMeshController.h"
//#include "AIAgentComponent.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "ModuleScene.h"
#include "DetourNavMeshQuery.h"
#include "Application.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "ResourceNavmesh.h"
#include "DetourCrowd.h"
#define MAX_AMOUNT 999


ModuleDetourNavigation::ModuleDetourNavigation()
{

}

ModuleDetourNavigation::~ModuleDetourNavigation()
{
	delete mNavMeshParams;
	delete mNavQuery;
}

bool ModuleDetourNavigation::Init()
{
	LoadResourceData();
	mCrowd = dtAllocCrowd();
	if (mDetourNavMesh)
	{
		mCrowd->init(mMaxAgents, mAgentRadius, mDetourNavMesh);
	}
	return true;
}

update_status ModuleDetourNavigation::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::Update(float dt)
{	
	if (mCrowd)
	{
		mCrowd->update(App->GetDt(), nullptr);
	}
	return UPDATE_CONTINUE;
}

update_status ModuleDetourNavigation::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleDetourNavigation::CleanUp()
{
	return true;
}

void ModuleDetourNavigation::LoadResourceData()
{
	std::string pathStr = std::string(ASSETS_NAVMESH_PATH);
	ResourceNavMesh* resource = (ResourceNavMesh*)App->GetResource()->RequestResource((pathStr + App->GetScene()->GetName() + ".navmesshi").c_str());
	if (resource)
	{
		mDetourNavMesh = resource->GetDtNavMesh();
		CreateQuery();
		App->GetResource()->ReleaseResource(resource->GetUID());
	}
}

void ModuleDetourNavigation::CreateQuery() {

	mNavQuery = new dtNavMeshQuery();
	dtStatus status;
	status = mNavQuery->init(mDetourNavMesh, 2048);
	if (dtStatusFailed(status))
	{
		LOG("Could not init Detour navmesh query");
		return;
	}
}

std::vector<float3> ModuleDetourNavigation::FindNavPath(float3 startPos, float3 endPos)
{
	if (!mNavQuery) 
	{

		LOG("BUILD NAVMESH");
		std::vector<float3> breakVector(0);
		return breakVector;
	}
	float3 queryAreafSize = float3(5.0f);

	dtPolyRef startPolygon;
	dtQueryFilter startTemp;
	float3 queryEndPos = float3(0.0f);
	mNavQuery->findNearestPoly(&startPos[0], &queryAreafSize[0], &startTemp, &startPolygon, &queryEndPos[0]);

	dtPolyRef endPolygon;
	dtQueryFilter endTemp;
	mNavQuery->findNearestPoly(&endPos[0], &queryAreafSize[0], &endTemp, &endPolygon, &queryEndPos[0]);

	dtQueryFilter pathFilter;
	dtPolyRef* polygonPath = new dtPolyRef[MAX_AMOUNT];
	int pathPoylgonCount = 0;

	mNavQuery->findPath(startPolygon, endPolygon, &startPos[0], &endPos[0], &pathFilter, polygonPath, &pathPoylgonCount, MAX_AMOUNT);

	float* positionPath = new float[MAX_AMOUNT];
	int numberOfPositions = 0;
	unsigned char* straightPathFlags = new unsigned char[MAX_AMOUNT];
	/*dtPolyRef* currentPoly = new dtPolyRef(MAX_AMOUNT);*/

	mNavQuery->findStraightPath(&startPos[0], &endPos[0], polygonPath, pathPoylgonCount, positionPath, straightPathFlags, nullptr, &numberOfPositions, MAX_AMOUNT, 0);

	std::vector<float3> positionsPathResult;
	positionsPathResult.reserve(numberOfPositions);

	for (size_t i = 0; i < numberOfPositions; ++i)
	{
		positionsPathResult.push_back({ positionPath[i * 3],positionPath[i * 3 + 1],positionPath[i * 3 + 2] });
	}
	delete[] positionPath;
	delete[] polygonPath;
	delete[] straightPathFlags;
	return positionsPathResult;

}


void ModuleDetourNavigation:: FindDebugPoint() {
	if (mNavQuery)
	{
		dtPolyRef result;
		dtQueryFilter temp;
		mNavQuery->findNearestPoly(&mQueryCenter[0], &mQueryHalfSize[0], &temp, &result, &mQueryResult[0]);
	}
}

float3 ModuleDetourNavigation::FindNearestPoint(float3 center, float3 halfSize) 
{
	float3 queryResult = float3(0.0f);
	if (!mNavQuery) 
	{
		LOG("BUILD NAVMESH");
		return queryResult;
	}
	dtPolyRef result;
	dtQueryFilter temp;

	mNavQuery->findNearestPoly(&center[0], &halfSize[0], &temp, &result, &queryResult[0]);
	return queryResult;
}

unsigned int ModuleDetourNavigation::AddAgent(float3 startPos)
{
	unsigned int my_id = mCrowd->getAgentCount();
	dtCrowdAgentParams agentParams;
	memset(&agentParams, 0, sizeof(agentParams));
	agentParams.radius = 0.6f; // Adjust based on your requirements
	agentParams.height = 2.0f; // Adjust based on your requirements
	agentParams.maxAcceleration = 8.0f;
	agentParams.maxSpeed = 3.5f;
	agentParams.collisionQueryRange = agentParams.radius * 12.0f;
	agentParams.pathOptimizationRange = agentParams.radius * 30.0f;
	agentParams.updateFlags = DT_CROWD_ANTICIPATE_TURNS | DT_CROWD_OBSTACLE_AVOIDANCE;
	agentParams.obstacleAvoidanceType = 3.0f;
	agentParams.separationWeight = 2.0f;

	int agentId = mCrowd->addAgent(&startPos[0], &agentParams);

	return my_id;

}

void ModuleDetourNavigation::SetAgentDestination(unsigned int agentId, float3 destination)
{
	dtPolyRef result;
	dtQueryFilter temp;
	float3 queryResult;
	float3 halfSize{ 3,3,3 };

	mNavQuery->findNearestPoly(&destination[0], &halfSize[0], &temp, &result, &queryResult[0]);

	mCrowd->requestMoveTarget(agentId,result, &destination[0]);
}

void ModuleDetourNavigation::MoveAgent(unsigned int agentId, float3& position)
{
	if (mCrowd)
	{
		if (agentId < mCrowd->getAgentCount())
		{
			const dtCrowdAgent* ag = mCrowd->getAgent(agentId);
			if (ag->active) {
				// Update your game object's position with the agent's position
				float3 newPosition(ag->npos[0], ag->npos[1], ag->npos[2]);
				position = newPosition;
				
			}
		}
	}
}


