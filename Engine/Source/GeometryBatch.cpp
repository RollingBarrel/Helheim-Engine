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

	mVboSize = mUniqueMeshes[0]->GetNumVertices() * mVertexSize;
	mEboSize = mUniqueMeshes[0]->GetNumIndices();

	

	glGenVertexArrays(1, &mVao);
	glGenBuffers(1, &mVbo);
	glGenBuffers(1, &mEbo);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboSize, mUniqueMeshes[0]->GetInterleavedData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mUniqueMeshes[0]->mNumIndices * sizeof(unsigned int), mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	


	//ESTO PETA
	//glGenBuffers(1, &mIbo);
	//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	//glBufferData(GL_DRAW_INDIRECT_BUFFER,  mCommands.size() * 20 , mCommands[0], GL_STATIC_DRAW);


	Command* newCommand = new Command();
	newCommand->mCount = mUniqueMeshes[0]->GetNumIndices();
	newCommand->mInstanceCount = 0;
	newCommand->firstIndex = 0;
	newCommand->baseVertex = 0;
	newCommand->baseInstance = 0;

	mCommands.push_back(newCommand);
	mesh->SetCommand(newCommand);

}

GeometryBatch::~GeometryBatch()
{

}



void GeometryBatch::AddMesh(MeshRendererComponent* cMesh)
{

	mMeshComponents.push_back(cMesh);
	ResourceMesh* rMesh = cMesh->GetResourceMesh();


	for (auto UniqueMesh : mUniqueMeshes) {

		//assert(UniqueMesh != mesh);
	}

	mUniqueMeshes.push_back(rMesh);
	

	Command* newCommand = new Command();
	newCommand->mCount = rMesh->GetNumIndices();
	newCommand->mInstanceCount = 0;
	newCommand->baseInstance = 0;



	unsigned int  destVbo;
	unsigned int newVboSize = mVboSize + (mVertexSize * rMesh->GetNumVertices());

	glGenBuffers(1, &destVbo);
	glBindBuffer(GL_ARRAY_BUFFER, destVbo);
	glBufferData(GL_ARRAY_BUFFER, newVboSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glCopyNamedBufferSubData(mVbo, destVbo, 0, 0, mVboSize);
	glDeleteBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, destVbo);
	glBufferSubData(GL_ARRAY_BUFFER, mVboSize, rMesh->GetNumVertices() * mVertexSize, rMesh->GetInterleavedData());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	newCommand->baseVertex = mVboSize;
	mVbo = destVbo;
	mVboSize = newVboSize;


	unsigned int  destEbo;
	unsigned int newEboSize = mEboSize + rMesh->GetNumIndices() * sizeof(unsigned int);


	glGenBuffers(1, &destEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, destEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newEboSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glCopyNamedBufferSubData(mEbo, destEbo, 0, 0, mEboSize);
	glDeleteBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, destEbo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, mEboSize, rMesh->GetNumIndices() * sizeof(unsigned int), rMesh->GetIndices());


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	newCommand->firstIndex = mEboSize;

	mEbo = destEbo;
	mEboSize = newEboSize;

}



void GeometryBatch::Draw()
{


	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mCommands, mCommands.size(), 0);


}
