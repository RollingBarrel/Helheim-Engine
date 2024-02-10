#include "NavMeshControllerComponent.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ModuleScene.h"
#include "Recast.h"
#include "Application.h"
#include "ImporterMesh.h"

NavMeshControllerComponent::NavMeshControllerComponent(GameObject* ownerGameObject)
	:Component(ownerGameObject, ComponentType::NAVMESHCONTROLLER)
{
	HandleBuild();
}

NavMeshControllerComponent::NavMeshControllerComponent(const NavMeshControllerComponent& original, GameObject* owner)
	:Component(owner, ComponentType::NAVMESHCONTROLLER)
{

}
void NavMeshControllerComponent::Reset() {
	//Change variables to default values.
}

void NavMeshControllerComponent::Update()
{
	MeshRendererComponent* c = nullptr;
		//c = mOwner->GetComponent<MeshRendererComponent>();
	c = (MeshRendererComponent*)mOwner->GetComponent(ComponentType::MESHRENDERER);
	
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
	const MeshRendererComponent* testMesh{mMeshRendererComponents[0]};
	
	if (testMesh) {
		 float maxX = testMesh->GetAABB().maxPoint.x;
		 float minX = testMesh->GetAABB().minPoint.x;
		 const float* maxPoint = static_cast<const float*>(&maxX);
		 const float* minPoint = static_cast<const float*>(&minX);
		 int gridWidth=0;
		 int gridHeight=0;

		rcCalcGridSize(minPoint, maxPoint,mCellSize, &gridWidth,&gridHeight);
		mHeightField = rcAllocHeightfield();
		if (!mHeightField)
		{
			LOG("buildNavigation: Out of memory 'mHeightField'.");
			return;
		}
		if (!rcCreateHeightfield(nullptr, *mHeightField, gridWidth,gridHeight, minPoint, maxPoint, mCellSize, mCellHeight))
		{
			LOG("buildNavigation: Could not create solid rcCreateHeightfield.");
			return;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		unsigned int numberOfTriangles = testMesh->GetResourceMesh()->mNumIndices;
		mTriangleAreas = new unsigned char[numberOfTriangles];
		if (!mTriangleAreas)
		{
			LOG("buildNavigation: Out of memory 'mTriangleAreas'");
			return;
		}
		float* vertices= (float*)(testMesh->GetResourceMesh()->GetAttributeData(Attribute::POS));
		int numberOfVertices = testMesh->GetResourceMesh()->mNumVertices;


		  int* triangle = (int*)(testMesh->GetResourceMesh()->mIndices);
		

		
		memset(mTriangleAreas, 0, numberOfTriangles * sizeof(unsigned char));
		rcMarkWalkableTriangles(nullptr, mMaxSlopeAngle,vertices , numberOfVertices,triangle, numberOfTriangles, mTriangleAreas);

		if (!rcRasterizeTriangles(nullptr, vertices, numberOfVertices, triangle, mTriangleAreas, numberOfTriangles, *mHeightField, 1))
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
			rcFilterLowHangingWalkableObstacles(nullptr, mWalkableClimb, *mHeightField);
		if (mFilterLedgeSpans)
			rcFilterLedgeSpans(nullptr, mWalkableHeight, mWalkableClimb, *mHeightField);
		if (mFilterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(nullptr, mWalkableHeight, *mHeightField);

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
		if (!rcBuildCompactHeightfield(nullptr, mWalkableHeight, mWalkableClimb, *mHeightField, *mCompactHeightField))
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
		if (!rcErodeWalkableArea(nullptr, mWalkableRadius,*mCompactHeightField))
		{
			LOG("buildNavigation: Could not erode.");
		}

		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(nullptr, *mCompactHeightField))
		{
			LOG("buildNavigation: Could not build distance field.");
			return;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(nullptr, *mCompactHeightField, 0, mMinRegionArea, mMergeRegionArea))
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
		if (!rcBuildContours(nullptr, *mCompactHeightField, mMaxSimplificationError, mMaxEdgeLen, *mContourSet))
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
		if (!rcBuildPolyMesh(nullptr, *mContourSet, mMaxVertsPerPoly, *mPolyMesh))
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

		if (!rcBuildPolyMeshDetail(nullptr, *mPolyMesh, *mCompactHeightField, mDetailSampleDist, mDetailSampleMaxError, *mPolyMeshDetail))
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
	for (const auto& child : gameObj->GetChildren()) {
		MeshRendererComponent* meshRendererComponent = gameObj->getMeshRenderer();
		if (meshRendererComponent) {
			mMeshesToNavMesh.push_back(meshRendererComponent->GetResourceMesh());
			mOBBs.push_back(meshRendererComponent->getOBB());
			mMeshRendererComponents.push_back(meshRendererComponent);
			
		}
		GetGOMeshes(child);
	}

}