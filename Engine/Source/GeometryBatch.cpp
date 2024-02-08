#include "Globals.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include <float3.h>
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"




GeometryBatch::GeometryBatch(MeshRendererComponent* mesh)
{
	mMeshComponents.push_back(mesh);
	
	mUniqueMeshes.push_back(mesh->GetResourceMesh());

	mAttributes = mesh->GetResourceMesh()->GetAttributes();

	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		mVertexSize += (*it)->size;
	}


	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);
	glGenBuffers(1, &mEbo);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mUniqueMeshes[0]->GetNumVertices() * mVertexSize, mUniqueMeshes[0]->GetInterleavedData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mUniqueMeshes[0]->mNumIndices * sizeof(unsigned int), mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	
}

GeometryBatch::~GeometryBatch()
{

}



void GeometryBatch::AddComponent(MeshRendererComponent* component)
{
	for (auto mesh : mMeshComponents) {

		assert(mesh != component);
	}

	mMeshComponents.push_back(component);

	AddResourceMesh(component->GetResourceMesh());

}

void GeometryBatch::AddResourceMesh(ResourceMesh* mesh)
{


	for (auto UniqueMesh : mUniqueMeshes) {

		assert(UniqueMesh != mesh);
	}

	mUniqueMeshes.push_back(mesh);


	//int size = 25;
	mVboSize = 2*4;

	unsigned int  destVbo;
	float num[2] = { 1.0f , 2.0f};

	
	unsigned int newVboSize = mVboSize + (mVertexSize * mesh->GetNumVertices());
	//unsigned int newVboSize = mVboSize * 2; //TESTING

	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboSize, &num, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &destVbo);
	glBindBuffer(GL_ARRAY_BUFFER, destVbo);
	glBufferData(GL_ARRAY_BUFFER, newVboSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glCopyNamedBufferSubData(mVbo, destVbo, 0, 0, mVboSize);

	glDeleteBuffers(1 , &mVbo);	

	glBindBuffer(GL_ARRAY_BUFFER, destVbo);
	
	float* ptr = reinterpret_cast<float*>(reinterpret_cast<char*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE)) + mVboSize);
	
	float* data = mesh->GetInterleavedData();

	for (int i = 0; i < mesh->GetNumVertices() * mVertexSize; ++i) {

		ptr[i] = data[i];

	}

	/*
	LOG("%f", ptr[2]);
	ptr[0] = 3.0f;
	LOG("%f", *ptr);

	ptr[1] = 4.0f;
	LOG("%f", ptr[1]);
	*/

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	mVbo = destVbo;
	mVboSize = newVboSize;

}
