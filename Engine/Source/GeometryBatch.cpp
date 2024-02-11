#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include <float3.h>
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"




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
	glGenBuffers(1, &mSsboModels);
	glGenBuffers(1, &mIbo);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboSize, mUniqueMeshes[0]->GetInterleavedData(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mUniqueMeshes[0]->mNumIndices * sizeof(unsigned int), mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);
	glBindVertexArray(0);
	



	Material* material = new Material();

	material->diffuseColor = mesh->GetMaterial()->mDiffuseFactor;
	material->diffuseTexture = mesh->GetMaterial()->mDiffuseTexture->openGlId;
	material->specularColor =  float4(mesh->GetMaterial()->mSpecularFactor, 0);
	material->specularTexture = mesh->GetMaterial()->mSpecularGlossinessTexture->openGlId;
	material->normalTexture = mesh->GetMaterial()->mNormalTexture->openGlId;
	material->shininess = mesh->GetMaterial()->mGlossinessFactor;
	material->hasDiffuseMap = mesh->GetMaterial()->mEnableDiffuseTexture;
	material->hasSpecularMap = mesh->GetMaterial()->mEnableSpecularGlossinessTexture;
	material->hasShininessMap = mesh->GetMaterial()->mEnableShinessMap;
	material->hasNormalMap = mesh->GetMaterial()->mEnableNormalMap;

	mMaterials.push_back(material);
	




	glGenBuffers(1, &mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material), mMaterials[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

GeometryBatch::~GeometryBatch()
{

}



void GeometryBatch::AddMesh(MeshRendererComponent* cMesh) //NEEDS A FULL REMAKE
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

void GeometryBatch::AddCommand(Command* command)
{
	mCommands.push_back(command);
}



void GeometryBatch::Draw()
{
	App->GetOpenGL()->BindSceneFramebuffer();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());
	glBindVertexArray(mVao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), mCommands[0], GL_STATIC_DRAW);



	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(float) * 16, nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels);
	
	unsigned int offset = 0;

	for (auto mesh : mMeshComponents) {

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float) * 16, mesh->GetOwner()->GetWorldTransform().Transposed().ptr());

		offset += sizeof(float) * 16;
	}


	for (auto material : mMaterials) {
		glMakeTextureHandleResidentARB(material->diffuseTexture);
		glMakeTextureHandleResidentARB(material->specularTexture);
		glMakeTextureHandleResidentARB(material->normalTexture);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);

	//glBindBuffer(GL_ARRAY_BUFFER, mIbo);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*) 0, mCommands.size(), 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glUseProgram(0);
	glBindVertexArray(0);

	for (auto material : mMaterials) {
		glMakeTextureHandleNonResidentARB(material->diffuseTexture);
		glMakeTextureHandleNonResidentARB(material->specularTexture);
		glMakeTextureHandleNonResidentARB(material->normalTexture);
	}

	App->GetOpenGL()->UnbindSceneFramebuffer();
}

