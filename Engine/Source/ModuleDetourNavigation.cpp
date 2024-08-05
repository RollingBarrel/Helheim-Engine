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
	delete mCrowd;
	mCrowd = nullptr;
	delete mNavQuery;
	mNavQuery = nullptr;
	if (mRNavMesh)
		App->GetResource()->ReleaseResource(mRNavMesh->GetUID());
}


bool ModuleDetourNavigation::Init()
{
	mCrowd = dtAllocCrowd();
	return true;
}


update_status ModuleDetourNavigation::Update(float dt)
{
	if (mCrowd && App->IsPlayMode())
	{
		if (mCrowd->getAgentCount() > 0)
		{
			mCrowd->update(App->GetDt(), nullptr);
		}
	}
	return UPDATE_CONTINUE;
}



unsigned int ModuleDetourNavigation::GetResourceId() const
{ 
	return (mRNavMesh) ? mRNavMesh->GetUID() : 0; 
}

void ModuleDetourNavigation::ReleaseResource()
{
	if (mRNavMesh)
		App->GetResource()->ReleaseResource(mRNavMesh->GetUID());
	mRNavMesh = nullptr;
}

void ModuleDetourNavigation::CreateQuery(unsigned int resourceId)
{
	ResourceNavMesh* newNavMesh = static_cast<ResourceNavMesh*>(App->GetResource()->RequestResource(resourceId, Resource::Type::NavMesh));
	assert(newNavMesh, "The saved scene navmesh resource id is incorrect");
	if (newNavMesh)
	{
		if (mNavQuery)
			delete mNavQuery;
		mNavQuery = new dtNavMeshQuery();
		dtStatus status;
		if (mRNavMesh)
			App->GetResource()->ReleaseResource(mRNavMesh->GetUID());
		mRNavMesh = newNavMesh;
		status = mNavQuery->init(mRNavMesh->GetDtNavMesh(), 2048);
		if (dtStatusFailed(status))
		{
			LOG("Could not init Detour navmesh query");
			return;
		}
		mCrowd->init(mMaxAgents, mAgentRadius, mRNavMesh->GetDtNavMesh());
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
	float3 queryEndPos = float3(0.0f);;
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

void ModuleDetourNavigation:: FindDebugPoint() 
{
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

unsigned int ModuleDetourNavigation::AddAgent(float3 startPos, dtCrowdAgentParams& params)
{
	int agentId = mCrowd->addAgent(&startPos[0], &params);
	return agentId >= 0 ? agentId : 101;
}

void ModuleDetourNavigation::SetAgentDestination(unsigned int agentId, float3 destination)
{
	dtPolyRef result;
	dtQueryFilter temp;
	float3 queryResult;
	float3 halfSize{ 10.0f };

	mNavQuery->findNearestPoly(&destination[0], &halfSize[0], &temp, &result, &queryResult[0]);

	mCrowd->requestMoveTarget(agentId, result, &destination[0]);
}

void ModuleDetourNavigation::MoveAgent(unsigned int agentId, float3& position)
{
	if (mCrowd)
	{
		if (agentId < mCrowd->getAgentCount())
		{
			const dtCrowdAgent* ag = mCrowd->getAgent(agentId);
			if (ag->active) 
			{
				// Update your game object's position with the agent's position
				float3 newPosition(ag->npos[0], ag->npos[1], ag->npos[2]);
				position = newPosition;

			}
		}
	}
}

void ModuleDetourNavigation::DisableAgent(unsigned int agentId)
{
	if (mCrowd != nullptr)
	{
		mCrowd->removeAgent(agentId);
	}
}

