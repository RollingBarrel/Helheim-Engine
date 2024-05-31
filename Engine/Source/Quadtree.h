#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>

#include "Geometry/AABB.h"
#include "MathGeoLibFwd.h"

class GameObject;
class MeshRendererComponent;
struct Hit;
#define MAX_DEPTH 6
#define CAPACITY 8

class ENGINE_API Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	bool AddObject(const MeshRendererComponent& mesh);
	void RemoveObject(const GameObject& object);
	bool Intersects(const OBB& boundingBox) const;
	bool Intersects(const Ray& ray) const;
	bool IsLeaf() const { return mChildren[0] == nullptr; }
	bool IsFilled() const { return mGameObjects.size() >= CAPACITY; }
	unsigned int GetNumGameObjs() const { return static_cast<unsigned int>(mGameObjects.size()); }
	const std::vector<const GameObject*>& GetGameObjects() const { return mGameObjects; }
	const Quadtree* GetChildren() const { return *mChildren; }
	void RayCast(const Ray& ray, std::map<float, Hit>& map) const;
	void UpdateTree();
	const char* GetName() const { return mName.c_str(); }
	void GetRenderComponentsInFrustum(const Frustum& cam, std::vector<const MeshRendererComponent*>& components) const;
	const AABB& GetBoundingBox() const { return mBoundingBox; }

	void CleanUp();

private:
	Quadtree(const AABB& boundingBox, int depth, std::string&& name);
	void SplitNode();
	void AddHierarchyObjects(const GameObject& child);

	AABB mBoundingBox;
	Quadtree* mChildren[4];
	std::vector<const GameObject*> mGameObjects;
	int mDepthLevel;
	std::string mName;
};

