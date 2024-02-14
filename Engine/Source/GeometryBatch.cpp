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
	mVertexSize = mesh->GetResourceMesh()->GetVertexSize();
	mesh->GetResourceMesh()->SetVboPosition(0);
	mesh->GetResourceMesh()->SetEboPosition(0);

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glGenBuffers(1, &mEbo);
	glGenBuffers(1, &mSsboModels);
	glGenBuffers(1, &mSsboMaterials);
	glGenBuffers(1, &mIbo);
	
	mVboSize = mesh->GetResourceMesh()->GetNumVertices() * mVertexSize;
	mEboSize = mUniqueMeshes[0]->mNumIndices * sizeof(unsigned int);


	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboSize, mUniqueMeshes[0]->GetInterleavedData(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboSize, mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);
	
	
/*
	Material* material = new Material();

	material->diffuseColor = mesh->GetMaterial()->mDiffuseFactor;
	material->diffuseTexture = mesh->GetMaterial()->mDiffuseTexture->mTextureHandle;
	material->specularColor =  float4(mesh->GetMaterial()->mSpecularFactor, 0);
	material->specularTexture = mesh->GetMaterial()->mSpecularGlossinessTexture->mTextureHandle;
	material->normalTexture = mesh->GetMaterial()->mNormalTexture->mTextureHandle;
	material->shininess = mesh->GetMaterial()->mGlossinessFactor;
	material->hasDiffuseMap = mesh->GetMaterial()->mEnableDiffuseTexture;
	material->hasSpecularMap = mesh->GetMaterial()->mEnableSpecularGlossinessTexture;
	material->hasShininessMap = mesh->GetMaterial()->mEnableShinessMap;
	material->hasNormalMap = mesh->GetMaterial()->mEnableNormalMap;

	mMaterials.push_back(material);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material), mMaterials[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
*/
	unsigned int idx = 0;
	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);
		
		++idx;
	}

	glBindVertexArray(0);

}

GeometryBatch::~GeometryBatch()
{

}


void GeometryBatch::AddMesh(MeshRendererComponent* cMesh)
{
	mMeshComponents.push_back(cMesh);


	bool found = false;
	for (auto mesh : mUniqueMeshes) {
		if (mesh->mUID == cMesh->GetResourceMesh()->mUID) {
			found = true;
		}
	}


	if (!found) {

		glBindVertexArray(mVao);
		mUniqueMeshes.push_back(cMesh->GetResourceMesh());
		unsigned int  destVbo;
		unsigned int newVboSize = mVboSize + (mVertexSize * cMesh->GetResourceMesh()->GetNumVertices());

		glGenBuffers(1, &destVbo);
		glBindBuffer(GL_ARRAY_BUFFER, destVbo);
		glBufferData(GL_ARRAY_BUFFER, newVboSize, nullptr, GL_STATIC_DRAW);
		glCopyNamedBufferSubData(mVbo, destVbo, 0, 0, mVboSize);
		glDeleteBuffers(1, &mVbo);


		glBufferSubData(GL_ARRAY_BUFFER, mVboSize, cMesh->GetResourceMesh()->GetNumVertices() * mVertexSize, cMesh->GetResourceMesh()->GetInterleavedData());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		cMesh->GetResourceMesh()->SetVboPosition(mVboSize);
		mVbo = destVbo;
		mVboSize = newVboSize;

		unsigned int destEbo;
		unsigned int newEboSize = mEboSize + cMesh->GetResourceMesh()->GetNumIndices() * sizeof(unsigned int);

		glGenBuffers(1, &destEbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, destEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, newEboSize, nullptr, GL_STATIC_DRAW);
		glCopyNamedBufferSubData(mEbo, destEbo, 0, 0, mEboSize);
		glDeleteBuffers(1, &mEbo);


		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, mEboSize, cMesh->GetResourceMesh()->GetNumIndices() * sizeof(unsigned int), cMesh->GetResourceMesh()->GetIndices());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		cMesh->GetResourceMesh()->SetEboPosition(mEboSize);
		mEbo = destEbo;
		mEboSize = newEboSize;
		glBindVertexArray(0);

	}











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
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), nullptr, GL_STATIC_DRAW);

	
	unsigned int offset3 = 0;
	for (auto command : mCommands) {
		glBufferSubData(GL_DRAW_INDIRECT_BUFFER, offset3, sizeof(Command), command);
		offset3 += sizeof(Command);
	}
	

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(float) * 16, nullptr, GL_STATIC_COPY_ARB);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels);
	
	unsigned int offset = 0;

	for (auto mesh : mMeshComponents) {

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float) * 16, mesh->GetOwner()->GetWorldTransform().ptr());
		offset += sizeof(float) * 16;
	}


	offset = 0;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(Material), nullptr, GL_STATIC_COPY_ARB);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);

	for (auto mesh : mMeshComponents) {

		Material* material = new Material();

		material->diffuseColor = mesh->GetMaterial()->mDiffuseFactor;
		material->diffuseTexture = mesh->GetMaterial()->mDiffuseTexture->mTextureHandle;
		material->specularColor = float4(mesh->GetMaterial()->mSpecularFactor, 0);
		material->specularTexture = mesh->GetMaterial()->mSpecularGlossinessTexture->mTextureHandle;
		material->normalTexture = mesh->GetMaterial()->mNormalTexture->mTextureHandle;
		material->shininess = mesh->GetMaterial()->mGlossinessFactor;
		material->hasDiffuseMap = mesh->GetMaterial()->mEnableDiffuseTexture;
		material->hasSpecularMap = mesh->GetMaterial()->mEnableSpecularGlossinessTexture;
		material->hasShininessMap = mesh->GetMaterial()->mEnableShinessMap;
		material->hasNormalMap = mesh->GetMaterial()->mEnableNormalMap;

		mMaterials.push_back(material);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), material);
		offset += sizeof(Material);

		glMakeTextureHandleResidentARB(material->diffuseTexture);
		glMakeTextureHandleResidentARB(material->specularTexture);
		glMakeTextureHandleResidentARB(material->normalTexture);
	}


	/*
	for (auto material : mMaterials) {
		glMakeTextureHandleResidentARB(material->diffuseTexture);
		glMakeTextureHandleResidentARB(material->specularTexture);
		glMakeTextureHandleResidentARB(material->normalTexture);
	}
	*/

	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0 , mCommands.size(), 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glUseProgram(0);
	glBindVertexArray(0);

	for (auto material : mMaterials) {
		glMakeTextureHandleNonResidentARB(material->diffuseTexture);
		glMakeTextureHandleNonResidentARB(material->specularTexture);
		glMakeTextureHandleNonResidentARB(material->normalTexture);
	}


	for (auto command : mCommands) {
		delete command;
	}

	for (auto material : mMaterials) {
		delete material;
	}

	mMaterials.clear();
	mCommands.clear();

	App->GetOpenGL()->UnbindSceneFramebuffer();


	
}

