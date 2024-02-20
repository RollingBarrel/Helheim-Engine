#include "NavMeshControllerComponent.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ModuleScene.h"

#include "Application.h"
#include "ImporterMesh.h"
#include "ModuleDebugDraw.h"

NavMeshControllerComponent::NavMeshControllerComponent(GameObject* ownerGameObject)
	:Component(ownerGameObject, ComponentType::NAVMESHCONTROLLER)
{
	mRecastContext = rcContext(false);
	
}

NavMeshControllerComponent::NavMeshControllerComponent(const NavMeshControllerComponent& original, GameObject* owner)
	:Component(owner, ComponentType::NAVMESHCONTROLLER)
{
	
}

NavMeshControllerComponent::~NavMeshControllerComponent()
{
}

void NavMeshControllerComponent::Reset() {
	//Change variables to default values.
}



void NavMeshControllerComponent::Update()
{
	HandleBuild();
	DebugDrawPolyMesh();
}

Component* NavMeshControllerComponent::Clone(GameObject* owner) const
{
    return new NavMeshControllerComponent(*this, owner); //Calls the copy contrustctor of your component
}

void NavMeshControllerComponent::Save(Archive& archive) const {
	archive.AddString("type", GetNameFromType());
}

void NavMeshControllerComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner) {
	
}

void NavMeshControllerComponent::HandleBuild() {
	GameObject* root= App->GetScene()->GetRoot();
	GetGOMeshes(root);
	mMeshRendererComponents;
	
	const MeshRendererComponent* testMesh{mMeshRendererComponents[0]};
	
	if (testMesh) {
		 float3 meshMax = testMesh->GetAABB().maxPoint;
		 float3 meshMin = testMesh->GetAABB().minPoint;
		 const float maxPoint[3] = { meshMax.x, meshMax.y, meshMax.z};
		 const float minPoint[3] = { meshMin.x, meshMin.y, meshMin.z };
		 int gridWidth=0;
		 int gridHeight=0;

		rcCalcGridSize(minPoint, maxPoint,mCellSize, &gridWidth,&gridHeight);
		mHeightField = rcAllocHeightfield();
		if (!mHeightField)
		{
			LOG("buildNavigation: Out of memory 'mHeightField'.");
			return;
		}
		if (!rcCreateHeightfield(&mRecastContext, *mHeightField, gridWidth,gridHeight, minPoint, maxPoint, mCellSize, mCellHeight))
		{
			LOG("buildNavigation: Could not create solid rcCreateHeightfield.");
			return;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		unsigned int numberOfTriangles = testMesh->GetResourceMesh()->mNumIndices/3;
		mTriangleAreas = new unsigned char[numberOfTriangles];
		if (!mTriangleAreas)
		{
			LOG("buildNavigation: Out of memory 'mTriangleAreas'");
			return;
		}
		float* vertices= (float*)(testMesh->GetResourceMesh()->GetAttributeData(Attribute::POS));
		int numberOfVertices = testMesh->GetResourceMesh()->mNumVertices;


		  const int* triangle = (const int*)(testMesh->GetResourceMesh()->mIndices);
		

		
		memset(mTriangleAreas, 0, numberOfTriangles * sizeof(unsigned char));
		rcMarkWalkableTriangles(&mRecastContext, mMaxSlopeAngle,vertices , numberOfVertices,triangle, numberOfTriangles, mTriangleAreas);

		if (!rcRasterizeTriangles(&mRecastContext, vertices, numberOfVertices, triangle, mTriangleAreas, numberOfTriangles, *mHeightField, 1))
		{
			LOG("buildNavigation: Could not rasterize triangles.");
			return;
		}

		if (!mKeepInterResults)
		{
			delete[] mTriangleAreas;
			mTriangleAreas = 0;
		}

		//
		// Step 3. Filter walkable surfaces.
		//

		// Once all geometry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (mFilterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(&mRecastContext, mWalkableClimb, *mHeightField);
		if (mFilterLedgeSpans)
			rcFilterLedgeSpans(&mRecastContext, mWalkableHeight, mWalkableClimb, *mHeightField);
		if (mFilterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(&mRecastContext, mWalkableHeight, *mHeightField);

		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		mCompactHeightField = rcAllocCompactHeightfield();
		if (!mCompactHeightField)
		{
			LOG("buildNavigation: Out of memory 'mCompactHeightField'.");
			return;
		}
		if (!rcBuildCompactHeightfield(&mRecastContext, mWalkableHeight, mWalkableClimb, *mHeightField, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not build compact data.");
			return;
		}

		if (!mKeepInterResults)
		{
			rcFreeHeightField(mHeightField);
			mHeightField = 0;
		}
		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(&mRecastContext, mWalkableRadius,*mCompactHeightField))
		{
			LOG("buildNavigation: Could not erode.");
		}

		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(&mRecastContext, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not build distance field.");
			return;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(&mRecastContext, *mCompactHeightField, 0, mMinRegionArea, mMergeRegionArea))
		{
			LOG("buildNavigation: Could not build watershed regions.");
			return;
		}

		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		mContourSet = rcAllocContourSet();
		if (!mContourSet)
		{
			LOG("buildNavigation: Out of memory 'mContourSet'.");
			return;
		}
		if (!rcBuildContours(&mRecastContext, *mCompactHeightField, mMaxSimplificationError, mMaxEdgeLen, *mContourSet))
		{
			LOG("buildNavigation: Could not create contours.");
			return;
		}


		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		mPolyMesh = rcAllocPolyMesh();
		if (!mPolyMesh)
		{
			LOG( "buildNavigation: Out of memory 'mPolyMesh'.");
			return;
		}
		if (!rcBuildPolyMesh(&mRecastContext, *mContourSet, mMaxVertsPerPoly, *mPolyMesh))
		{
			LOG( "buildNavigation: Could not triangulate contours.");
			return;
		}
		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		mPolyMeshDetail = rcAllocPolyMeshDetail();
		if (!mPolyMeshDetail)
		{
			LOG( "buildNavigation: Out of memory 'mPolyMeshDetail'.");
			return;
		}

		if (!rcBuildPolyMeshDetail(&mRecastContext, *mPolyMesh, *mCompactHeightField, mDetailSampleDist, mDetailSampleMaxError, *mPolyMeshDetail))
		{
			LOG("buildNavigation: Could not build detail mesh.");
			return;
		}

		if (!mKeepInterResults)
		{
			rcFreeCompactHeightfield(mCompactHeightField);
			mCompactHeightField = 0;
			rcFreeContourSet(mContourSet);
			mContourSet = 0;
		}



	}

	
}

void NavMeshControllerComponent::GetGOMeshes(const GameObject* gameObj){
	if (!(gameObj->GetChildren().empty())) {
		for (const auto& child : gameObj->GetChildren()) {
			MeshRendererComponent* meshRendererComponent = child->getMeshRenderer();
			if (meshRendererComponent) {
				mMeshesToNavMesh.push_back(meshRendererComponent->GetResourceMesh());
				mOBBs.push_back(meshRendererComponent->getOBB());
				mMeshRendererComponents.push_back(meshRendererComponent);

			}
			GetGOMeshes(child);
		}
	}

}

void NavMeshControllerComponent::DebugDrawPolyMesh()
{
	if (mDraw && mPolyMeshDetail != nullptr)
	{
		int triangle_index = 0;
		for (int i = 0; i < mPolyMeshDetail->ntris; ++i) {
			// Assuming tris are stored as indices to vertices
			unsigned char* triIndices = &mPolyMeshDetail->tris[i * 4];

			int vertIndex1 = triIndices[0];
			int vertIndex2 = triIndices[1];
			int vertIndex3 = triIndices[2];

			float* vertex1 = &mPolyMeshDetail->verts[vertIndex1 * 3];
			float* vertex2 = &mPolyMeshDetail->verts[vertIndex2 * 3];
			float* vertex3 = &mPolyMeshDetail->verts[vertIndex3 * 3];

			float3 v1 = float3(vertex1[0], vertex1[1], vertex1[2]);
			float3 v2 = float3(vertex2[0], vertex2[1], vertex2[2]);
			float3 v3 = float3(vertex3[0], vertex3[1], vertex3[2]);

			App->GetDebugDraw()->DrawTriangle(v1, v2, v3);

		}

	}
	return;
}
