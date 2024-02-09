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
	


	


	Command* newCommand = new Command();
	newCommand->mCount = mUniqueMeshes[0]->GetNumIndices();
	newCommand->mInstanceCount = 0;
	newCommand->firstIndex = 0;
	newCommand->baseVertex = 0;
	newCommand->baseInstance = 0;

	mCommands.push_back(newCommand);
	mesh->SetCommand(newCommand);


	
	glGenBuffers(1, &mIbo);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER,  mCommands.size() * sizeof(Command), mCommands[0], GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

}

GeometryBatch::~GeometryBatch()
{

}



void GeometryBatch::AddMesh(MeshRendererComponent* cMesh)
{
	mMeshComponents.push_back(cMesh);
	ResourceMesh* rMesh = cMesh->GetResourceMesh();

	bool found = false;
	Command* newCommand = new Command();
	newCommand->mCount = rMesh->GetNumIndices();


	for (auto UniqueMesh : mUniqueMeshes) {

		if (UniqueMesh != cMesh->GetResourceMesh()) {
			found = true;
		}
	}

	if (found) {

		cMesh->SetCommand(newCommand);
		mCommands.push_back(newCommand);
		newCommand->mInstanceCount = 0;
		newCommand->baseInstance = 0;
		newCommand->baseVertex = rMesh->GetVboPosition();
		newCommand->firstIndex = rMesh->GetEboPosition();
	}
	else {

		mUniqueMeshes.push_back(rMesh);
		cMesh->SetCommand(newCommand);
		mCommands.push_back(newCommand);
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
		rMesh->SetVboPosition(mVboSize);
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
		rMesh->SetEboPosition(mEboSize);
		//newCommand->firstIndex = 0;

		mEbo = destEbo;
		mEboSize = newEboSize;

	}



		unsigned int  destIbo;

		glGenBuffers(1, &destIbo);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, destIbo);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), mCommands[0], GL_STATIC_DRAW);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glCopyNamedBufferSubData(mIbo, destIbo, 0, 0, (mCommands.size() - 1) * sizeof(Command));
		glDeleteBuffers(1, &mIbo);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, destIbo);
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, (mCommands.size() - 1) * sizeof(Command), sizeof(Command), newCommand);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		mIbo = destIbo;
	
}



void GeometryBatch::Draw()
{


	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*) 0, mCommands.size(), 0);


}
