#include "NavMeshController.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "MeshRendererComponent.h"
#include "ModuleDetourNavigation.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "float4x4.h"
#include "ImporterMesh.h"
#include "ModuleDebugDraw.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "Recast.h"
#include "Tag.h"
#include "ModuleFileSystem.h"
#include "Algorithm/Random/LCG.h"
#include "ImporterNavMesh.h"
#include "ModuleResource.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
#include "ResourceNavmesh.h"
NavMeshController::NavMeshController()
{
}

NavMeshController::~NavMeshController()
{
	mVertices.clear();
	mIndices.clear();
	mGameObjects.clear();

	delete mPolyMesh;
	delete mPolyMeshDetail;
}

void NavMeshController::Reset() {
	//Change variables to default values.
}

void NavMeshController::TranslateIndices()
{
	for (int i = 0; i < mPolyMeshDetail->nmeshes; ++i)
	{
		const unsigned int* m = &mPolyMeshDetail->meshes[i * 4];
		const unsigned int bverts = m[0];
		const unsigned int btris = m[2];
		const int ntris = (int)m[3];
		const float* verts = &mPolyMeshDetail->verts[bverts * 3];
		const unsigned char* tris = &mPolyMeshDetail->tris[btris * 4];

		for (int j = 0; j < ntris; ++j)
		{
			float3 a = float3(verts[tris[j * 4 + 0] * 3], verts[tris[j * 4 + 0] * 3 + 1], verts[tris[j * 4 + 0] * 3 + 2]);
			float3 b = float3(verts[tris[j * 4 + 1] * 3], verts[tris[j * 4 + 1] * 3 + 1], verts[tris[j * 4 + 1] * 3 + 2]);
			float3 c = float3(verts[tris[j * 4 + 2] * 3], verts[tris[j * 4 + 2] * 3 + 1], verts[tris[j * 4 + 2] * 3 + 2]);

			// Check and update indices				
			int index = mVertices.size();	//This fcking warrada (starts at 0, next iteration size is +3, cause 3 vertex are added)
			mVertices.push_back(a);			
			mVertices.push_back(b);
			mVertices.push_back(c);

			// Add indices to mIndices
			mIndices.push_back(index);
			mIndices.push_back(index+1);
			mIndices.push_back(index+2);

		}
	}
}

void NavMeshController::HandleBuild() 
{
	mIndices.clear();
	mVertices.clear();
	mGameObjects.clear();
	mObstaclesTriangles.clear();


	GameObject* root = App->GetScene()->GetRoot();
	GetGOMeshes(root);
	if (mGameObjects.empty())
		return;
	std::vector<rcPolyMeshDetail*> myPolyMeshDetails;
	std::vector<rcPolyMesh*> myPolyMeshes;

	rcHeightfield* heightField = nullptr;
	rcCompactHeightfield* compactHeightField = nullptr;
	rcContourSet* contourSet = nullptr;
	rcContext* recastContext = new rcContext();
	unsigned char* triangleAreas = nullptr;


	const MeshRendererComponent* testMesh;
	int indicesSize = 0;
	int verticesSize = 0;
	for (int index = 0; index < mGameObjects.size(); index++) 
	{
		MeshRendererComponent* meshRenderer = (MeshRendererComponent*)(mGameObjects[index]->GetComponent(ComponentType::MESHRENDERER));

		indicesSize += meshRenderer->GetResourceMesh()->GetNumberIndices();
		verticesSize += meshRenderer->GetResourceMesh()->GetNumberVertices();
	}

	std::vector<float3> vertices;
	vertices.reserve(verticesSize);
	std::vector<int>indices;
	indices.reserve(indicesSize);
	int lastIndex = 0;
	int lastVertex = 0;
	for (int index = 0; index < mGameObjects.size(); index++) 
	{
		testMesh = (MeshRendererComponent*)(mGameObjects[index]->GetComponent(ComponentType::MESHRENDERER));

		if (!testMesh)
		{
			LOG("A mesh was not correctly loaded to the navigation controller.");
			break;
		}
		int meshIndiceNumber = testMesh->GetResourceMesh()->GetNumberIndices();
		const unsigned int* meshIndices = testMesh->GetResourceMesh()->GetIndices();
		Tag* goTag = mGameObjects[index]->GetTag();
		if (goTag && goTag->GetName() == "Obstacle") 
		{

			ObstacleTriangle obstacle{ lastIndex/3,meshIndiceNumber };
			mObstaclesTriangles.push_back(obstacle);
		}

		for (int i = 0; i < meshIndiceNumber; i++) 
		{
			indices.push_back(meshIndices[i]+lastVertex);
		}
		lastIndex += meshIndiceNumber;


		int meshVertiSize = testMesh->GetResourceMesh()->GetNumberVertices();
		float3* meshVertices = (float3*)(testMesh->GetResourceMesh()->GetAttributeData(Attribute::POS));
		float4x4 objectTransform = mGameObjects[index]->GetWorldTransform();
		for (int i = 0; i < meshVertiSize; ++i)
		{
			float3 vertTransformed = objectTransform.TransformPos(meshVertices[i]);
			vertices.push_back(vertTransformed);
		}

		lastVertex += meshVertiSize;
	}

	mAABB.SetFrom(&vertices[0], verticesSize);
	mOBB.SetFrom(mAABB, float4x4::identity);
	AABB AABBWorld = mOBB.MinimalEnclosingAABB();
	float3 meshMax = AABBWorld.maxPoint;
	float3 meshMin = AABBWorld.minPoint;
	const float maxPoint[3] = { meshMax.x, meshMax.y, meshMax.z };
	const float minPoint[3] = { meshMin.x, meshMin.y, meshMin.z };
	int gridWidth = 0;
	int gridHeight = 0;

	rcCalcGridSize(minPoint, maxPoint, mCellSize, &gridWidth, &gridHeight);
	heightField = rcAllocHeightfield();
	if (!heightField)
	{
		LOG("buildNavigation: Out of memory 'mHeightField'.");
		return;
	}
	if (!rcCreateHeightfield(recastContext, *heightField, gridWidth, gridHeight, minPoint, maxPoint, mCellSize, mCellHeight))
	{
		LOG("buildNavigation: Could not create solid rcCreateHeightfield.");
		return;

	}
	unsigned int numberOfTriangles = indicesSize / 3;
	triangleAreas = new unsigned char[numberOfTriangles];
	if (!triangleAreas)
	{
		LOG("buildNavigation: Out of memory 'mTriangleAreas'");
		return;
	}

	memset(triangleAreas, 0, numberOfTriangles * sizeof(unsigned char));
	rcMarkWalkableTriangles(recastContext, mMaxSlopeAngle, vertices[0].ptr(), verticesSize, &indices[0], numberOfTriangles, triangleAreas);

	//Check manually if htere is obstacle and make them not count towards the navmesh
	for (const auto& obstacleTriangle : mObstaclesTriangles)
	{
		int lastObstacleIndex = obstacleTriangle.startIndicePos + obstacleTriangle.numberOfIndices/3;
		for (size_t i = obstacleTriangle.startIndicePos; i < lastObstacleIndex; i++)
		{
			triangleAreas[i] = 0;
		}
	}

	if (!rcRasterizeTriangles(recastContext,vertices[0].ptr(), verticesSize, &indices[0], triangleAreas, numberOfTriangles, *heightField, 1))
	{
		LOG("buildNavigation: Could not rasterize triangles.");
		return;
	}
		delete[] triangleAreas;
		triangleAreas = 0;

	//
	// Step 3. Filter walkable surfaces.
	//

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.

		rcFilterLowHangingWalkableObstacles(recastContext, mWalkableClimb, *heightField);
		rcFilterLedgeSpans(recastContext, mWalkableHeight, mWalkableClimb, *heightField);
		rcFilterWalkableLowHeightSpans(recastContext, mWalkableHeight, *heightField);

	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	compactHeightField = rcAllocCompactHeightfield();
	if (!compactHeightField)
	{
		LOG("buildNavigation: Out of memory 'mCompactHeightField'.");
		return;
	}
	if (!rcBuildCompactHeightfield(recastContext, mWalkableHeight, mWalkableClimb, *heightField, *compactHeightField))
	{
		LOG("buildNavigation: Could not build compact data.");
		return;
	}


		rcFreeHeightField(heightField);
		heightField = 0;

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(recastContext, mWalkableRadius, *compactHeightField))
	{
		LOG("buildNavigation: Could not erode.");
	}

	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(recastContext, *compactHeightField))
	{
		LOG("buildNavigation: Could not build distance field.");
		return;
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(recastContext, *compactHeightField, 0, mMinRegionArea, mMergeRegionArea))
	{
		LOG("buildNavigation: Could not build watershed regions.");
		return;
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	contourSet = rcAllocContourSet();
	if (!contourSet)
	{
		LOG("buildNavigation: Out of memory 'mContourSet'.");
		return;
	}
	if (!rcBuildContours(recastContext, *compactHeightField, mMaxSimplificationError, mMaxEdgeLen, *contourSet))
	{
		LOG("buildNavigation: Could not create contours.");
		return;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	rcPolyMesh* tempPolyMesh = rcAllocPolyMesh();
	if (!tempPolyMesh)
	{
		LOG("buildNavigation: Out of memory 'tempPolyMesh'.");
		return;
	}
	if (!rcBuildPolyMesh(recastContext, *contourSet, mMaxVertsPerPoly, *tempPolyMesh))
	{
		LOG("buildNavigation: Could not triangulate contours.");
		return;
	}
	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//
	rcPolyMeshDetail* tempPolyMeshDetail = rcAllocPolyMeshDetail();
	if (!tempPolyMeshDetail)
	{
		LOG("buildNavigation: Out of memory 'tempPolyMeshDetail'.");
		return;
	}

	if (!rcBuildPolyMeshDetail(recastContext, *tempPolyMesh, *compactHeightField, mDetailSampleDist, mDetailSampleMaxError, *tempPolyMeshDetail))
	{
		LOG("buildNavigation: Could not build detail mesh.");
		return;
	}
	//This makes the polygon walkable for the Detour library, per default they are not walkable so you have to put them manually
	if (tempPolyMesh->npolys > 0)
	{
		for (int i = 0; i < tempPolyMesh->npolys; ++i)
		{
			if (tempPolyMesh->areas[i] == RC_WALKABLE_AREA)
			{
				tempPolyMesh->flags[i] = 1;
			}
			else
			{
				tempPolyMesh->flags[i] = 0;
			}
		}

		if (tempPolyMesh->npolys > 0)
		{
			for (int i = 0; i < tempPolyMesh->npolys; ++i)
			{
				if (tempPolyMesh->areas[i] == RC_WALKABLE_AREA)
				{
					tempPolyMesh->flags[i] = 1;
				}
				else
				{
					tempPolyMesh->flags[i] = 0;
				}
			}

			myPolyMeshDetails.push_back(tempPolyMeshDetail);
			myPolyMeshes.push_back(tempPolyMesh);
		}
		else
		{
			rcFreePolyMesh(tempPolyMesh);
			rcFreePolyMeshDetail(tempPolyMeshDetail);
		}

		rcFreeCompactHeightfield(compactHeightField);
		compactHeightField = 0;
		rcFreeContourSet(contourSet);
		contourSet = 0;

		mPolyMesh = rcAllocPolyMesh();
		if (!mPolyMesh)
		{
			LOG("buildNavigation: Out of memory 'mPolyMesh'.");
			return;
		}
		mPolyMeshDetail = rcAllocPolyMeshDetail();
		if (!mPolyMeshDetail)
		{
			LOG("buildNavigation: Out of memory 'mPolyMeshDetail'.");
			return;
		}

		if (!rcMergePolyMeshes(recastContext, &myPolyMeshes[0], myPolyMeshes.size(), *mPolyMesh))
		{
			LOG("mergePolymeshes: Failed to merge polymeshes.");
			return;
		}

		if (!rcMergePolyMeshDetails(recastContext, &myPolyMeshDetails[0], myPolyMeshDetails.size(), *mPolyMeshDetail))
		{
			LOG("mergePolymeshdetails: Failed to merge polymeshdetails.");
			return;
		}
		TranslateIndices();
		CreateDetourData();


	}

	delete recastContext;
}

void NavMeshController::CreateDetourData()
{
	//const AIAgentComponent* agentComponent = mAIAgentComponents[0];
	dtNavMeshCreateParams* mNavMeshParams = new dtNavMeshCreateParams();

	if (!mPolyMesh) return;
	unsigned char* navData = 0;
	int navDataSize = 0;
	/*if (agentComponent) {*/

	mNavMeshParams->verts = mPolyMesh->verts;
	mNavMeshParams->vertCount = mPolyMesh->nverts;
	mNavMeshParams->polys = mPolyMesh->polys;
	mNavMeshParams->polyAreas = mPolyMesh->areas;
	mNavMeshParams->polyFlags = mPolyMesh->flags;
	mNavMeshParams->polyCount = mPolyMesh->npolys;
	mNavMeshParams->nvp = mPolyMesh->nvp;
	mNavMeshParams->detailMeshes = mPolyMeshDetail->meshes;
	mNavMeshParams->detailVerts = mPolyMeshDetail->verts;
	mNavMeshParams->detailVertsCount = mPolyMeshDetail->nverts;
	mNavMeshParams->detailTris = mPolyMeshDetail->tris;
	mNavMeshParams->detailTriCount = mPolyMeshDetail->ntris;
	mNavMeshParams->offMeshConVerts = nullptr;
	mNavMeshParams->offMeshConRad = nullptr;
	mNavMeshParams->offMeshConDir = nullptr;
	mNavMeshParams->offMeshConAreas = nullptr;
	mNavMeshParams->offMeshConFlags = nullptr;
	mNavMeshParams->offMeshConUserID = nullptr;
	mNavMeshParams->offMeshConCount = 0;
	mNavMeshParams->walkableHeight = 1.0f;
	mNavMeshParams->walkableRadius = 0.5f;
	mNavMeshParams->walkableClimb = 0.0f;
	rcVcopy(mNavMeshParams->bmin, mPolyMesh->bmin);
	rcVcopy(mNavMeshParams->bmax, mPolyMesh->bmax);
	mNavMeshParams->cs = mCellSize;
	mNavMeshParams->ch = mCellHeight;
	mNavMeshParams->buildBvTree = true;
	
	if (!dtCreateNavMeshData(mNavMeshParams, &navData, &navDataSize))
	{
		LOG("Could not build Detour navmesh.");
		return;
	}
	 mDetourNavMesh = dtAllocNavMesh();
	if (!mDetourNavMesh)
	{
		dtFree(navData);
		LOG("Could not create Detour navmesh");
		return;
	}
	
	dtStatus status;
	status = mDetourNavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navData);
		LOG("Could not init Detour navmesh");
		return;
	}

	App->GetNavigation()->SetDetourNavMesh(mDetourNavMesh);
	
	Resource* resource = EngineApp->GetEngineResource()->CreateNewResource(nullptr, nullptr, Resource::Type::NavMesh);
	delete resource;
	
	App->GetNavigation()->CreateQuery();
}

void NavMeshController::GetGOMeshes(const GameObject* gameObj) {
	if (!(gameObj->GetChildren().empty())) 
	{
		for (const auto& child : gameObj->GetChildren()) 
		{
			MeshRendererComponent* meshRendererComponent = (MeshRendererComponent*)(child->GetComponent(ComponentType::MESHRENDERER));
			if (meshRendererComponent) 
			{
				mGameObjects.push_back(child);
			}
			GetGOMeshes(child);
		}
	}

}





int NavMeshController::FindVertexIndex(float3 vert)
{

	for (int i = 0; i < mVertices.size(); ++i) 
	{
		if (mVertices[i].x == vert.x && mVertices[i].y == vert.y && mVertices[i].z == vert.z) {
			return i;
		}
	}
	return -1; // Not found

}
