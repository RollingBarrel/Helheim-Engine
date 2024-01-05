#include "Quadtree.h"
#include "GameObject.h"
#include "Geometry/OBB.h"
#include "Geometry/AABB2D.h"
#include "MeshRendererComponent.h"

Quadtree::Quadtree(int depth, AABB2D bounding_box)
{
	has_been_filled = false;
	depth_level = depth;
	mBoundingBox = bounding_box;
}

bool Quadtree::AddObject(GameObject* object)
{
	OBB object_BB = object->getMeshRenderer()->getOBB();
	AABB2D projected_object_BB = project2D(&object_BB);
	if (!mBoundingBox.Intersects(projected_object_BB))
		return false;


	if (mGameObjects.size() >= CAPACITY || has_been_filled) {

		if (depth_level >= MAX_DEPTH) {
			mGameObjects.push_back(object);
			return true;
		}
		if(!has_been_filled)
			SplitNode();
		children[0]->AddObject(object);
		children[1]->AddObject(object);
		children[2]->AddObject(object);
		children[3]->AddObject(object);
		has_been_filled = true;
		return true;

	}

	mGameObjects.push_back(object);
	return true;

}

void Quadtree::RemoveObject(GameObject* object)
{
	if (has_been_filled) {
		children[0]->RemoveObject(object);
		children[1]->RemoveObject(object);
		children[2]->RemoveObject(object);
		children[3]->RemoveObject(object);

	}
	else {
		mGameObjects.erase(std::remove_if(mGameObjects.begin(), mGameObjects.end(),
			[object](GameObject* ptr) { return ptr == object; }),
			mGameObjects.end());
	}

}

bool Quadtree::Intersects(OBB* bounding_box)
{
	AABB2D projected_BB = project2D(bounding_box);
	return mBoundingBox.Intersects(projected_BB);
}

void Quadtree::SplitNode()
{
	float2 minPoint = mBoundingBox.minPoint;
	float2 maxPoint = mBoundingBox.maxPoint;

	float2 center = (minPoint + maxPoint) * 0.5f;

	// Calculate points on each side
	float2 midTop = float2(center.x, maxPoint.y);
	float2 midBottom = float2(center.x, minPoint.y);
	float2 midLeft = float2(minPoint.x, center.y);
	float2 midRight = float2(maxPoint.x, center.y);


	children[0] = new Quadtree(depth_level + 1, AABB2D(midLeft, midTop));
	children[1] = new Quadtree(depth_level + 1, AABB2D(center, maxPoint));
	children[2] = new Quadtree(depth_level + 1, AABB2D(minPoint, center));
	children[3] = new Quadtree(depth_level + 1, AABB2D(midBottom, midRight));

	for (const auto& object : mGameObjects) {
		children[0]->AddObject(object);
		children[1]->AddObject(object);
		children[2]->AddObject(object);
		children[3]->AddObject(object);

	}

	mGameObjects.clear();

}

const AABB2D Quadtree::project2D(OBB* bounding_box) const
{
	
	//TODO: See how we project objects to 2D without creating Axis Aligned Boxes.
	return AABB2D();
}
