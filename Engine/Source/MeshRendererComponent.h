#pragma once
#include "Component.h"
//#include "Mesh.h"
#include "Geometry/AABB.h"
class MeshRendererComponent : public Component
{
public:
	void Draw();
	void Load();


private:
	
	void LoadVBO();
	void LoadEBO();
	void LoadVAO();


	//Mesh mMesh
	AABB mAABB;

	

};

