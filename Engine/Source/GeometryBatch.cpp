#include "GeometryBatch.h"
#include <cassert>


GeometryBatch::GeometryBatch(std::vector<Attribute*> attributes)
	:mAttributes(attributes)
{
}

void GeometryBatch::AddComponent(MeshRendererComponent* component)
{
	for (auto mesh : mMeshComponents) {

		assert(mesh != component);
	}

	mMeshComponents.push_back(component);

}

void GeometryBatch::AddResourceMesh(ResourceMesh* mesh)
{


	for (auto UniqueMesh : mUniqueMeshes) {

		assert(UniqueMesh != mesh);
	}

	mUniqueMeshes.push_back(mesh);


}
