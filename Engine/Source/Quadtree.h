#pragma once

#include <vector>
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

class GameObject;
class MeshRendererComponent;

#define MAX_DEPTH 8
#define CAPACITY 4


class Quadtree
{
public:
	Quadtree(int depth, AABB bounding_box);
	~Quadtree();

	bool AddObject(GameObject* object);
	void RemoveObject(GameObject* object);
	bool Intersects(OBB* bounding_box);
	const bool hasGameObjects() const { return has_been_filled; }
	const int GetNumGameObjs() const { return mGameObjects.size(); }
	void CleanUp();
	void Draw();

private:

	void SplitNode(); //Quan es detecta que tenim més del maxim de game objects permès cridem aixo per buidar tot 
	//i crear els children i posar els game objects on pertany.

	AABB mBoundingBox;
	Quadtree* children[4];
	std::vector<GameObject*> mGameObjects;
	int depth_level;
	bool has_been_filled;

};

