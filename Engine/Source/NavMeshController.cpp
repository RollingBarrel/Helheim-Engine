#include "NavMeshController.h"
#include "GameObject.h"
#include "ResourceMesh.h"
#include "MeshRendererComponent.h"
#include "ModuleDetourNavigation.h"
#include "ModuleScene.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "glew.h"
#include "float4x4.h"
#include "ImporterMesh.h"
#include "ModuleDebugDraw.h"
#include "DetourNavMeshBuilder.h"
#include "Geometry/Triangle.h"
#include "Recast.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Tag.h"
#include "ModuleFileSystem.h"
#include "Algorithm/Random/LCG.h"
#include "ImporterNavMesh.h"
#include "ModuleResource.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
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

void NavMeshController::DebugDrawPolyMesh()
{
	if (!mDraw)
		return;

	if (mPolyMeshDetail == nullptr)
		return;
	if (mPolyMeshDetail->nmeshes < 1)
		return;
	
	
	unsigned int program = App->GetOpenGL()->GetDebugDrawProgramId();
	float4x4 identity = float4x4::identity;
	float4x4 view = ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetViewMatrix();
	float4x4 proj = ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetProjectionMatrix();

	GLint viewLoc = glGetUniformLocation(program, "view");
	GLint projLoc = glGetUniformLocation(program, "proj");
	GLint modelLoc = glGetUniformLocation(program, "model");
	glBindVertexArray(0);
	glUseProgram(0);
	glUseProgram(program);

	glUniformMatrix4fv(viewLoc, 1, GL_TRUE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_TRUE, &proj[0][0]);
	glUniformMatrix4fv(modelLoc, 1, GL_TRUE, &identity[0][0]);

	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
//	float3 color = float3(1.0f, 0.0f, 0.0f);
//App->GetDebugDraw()->DrawSphere(&mQueryResult[0], &color[0], 1.0f);
//float3 color2 = float3(1.0f, 1.0f, 0.0f);
//App->GetDebugDraw()->DrawSphere(&mQueryCenter[0], &color2[0], 1.0f);
//float3 color3 = float3(0.0f, 0.0f, 1.0f);
//float3 minAABB = mQueryCenter - mQueryHalfSize;
//float3 maxAABB = mQueryCenter + mQueryHalfSize;
//OBB cube = OBB(AABB(minAABB, maxAABB));
//App->GetDebugDraw()->DrawCube(cube, color3);


}

void NavMeshController::Update()
{
	if (mPolyMesh == nullptr)
		return;

	App->GetOpenGL()->BindSceneFramebuffer();
	DebugDrawPolyMesh();
	App->GetOpenGL()->UnbindSceneFramebuffer();

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
	mAABBWorld = mOBB.MinimalEnclosingAABB();
	float3 meshMax = mAABBWorld.maxPoint;
	float3 meshMin = mAABBWorld.minPoint;
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
		LoadDrawMesh();
		


		App->GetNavigation()->CreateDetourData();
		Resource* resource = EngineApp->GetEngineResource()->CreateNewResource(nullptr, nullptr, Resource::Type::NavMesh);
		delete resource;

	}

	delete recastContext;
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



void NavMeshController::LoadDrawMesh()
{
	if (mPolyMeshDetail != nullptr)
	{
		if (mPolyMeshDetail->nmeshes < 1)
			return; // Maybe remove the vao from memory until new call? Warn user?

		// Now you can create the VAO and fill it with the mesh data
		glGenVertexArrays(1, &mVao);
		glBindVertexArray(mVao);


		glGenBuffers(1, &mVbo);
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, 3 * mVertices.size() * sizeof(float), &mVertices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &mEbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);


		glBindVertexArray(0);

	}
	return;
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
