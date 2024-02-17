#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include "float3.h"
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

	ResourceMesh& rMesh = const_cast<ResourceMesh&>(*mesh->GetResourceMesh());
	rMesh.SetVboPosition(0);
	rMesh.SetEboPosition(0);

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
	float* interleavedData = mUniqueMeshes[0]->GetInterleavedData();
	glBufferData(GL_ARRAY_BUFFER, mVboSize, interleavedData, GL_STATIC_DRAW);
	delete[] interleavedData;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboSize, mUniqueMeshes[0]->GetIndices(), GL_STATIC_DRAW);

	unsigned int idx = 0;
	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);
		
		++idx;
	}

	glBindVertexArray(0);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

GeometryBatch::~GeometryBatch()
{

}


void GeometryBatch::AddMesh(MeshRendererComponent* cMesh)
{
	mMeshComponents.push_back(cMesh);

	ResourceMesh& rMesh = const_cast<ResourceMesh&>(*cMesh->GetResourceMesh());
	for (const ResourceMesh* mesh : mUniqueMeshes) {
		if (mesh->mUID == rMesh.mUID) {
			rMesh.SetVboPosition(mesh->GetVboPosition());
			rMesh.SetEboPosition(mesh->GetEboPosition());
			return;
		}
	}

	mUniqueMeshes.push_back(&rMesh);

	unsigned int  destVbo, destEbo;
	unsigned int newVboSize = mVboSize + (mVertexSize * rMesh.GetNumVertices());
	unsigned int newEboSize = mEboSize + rMesh.GetNumIndices() * sizeof(unsigned int);


	glBindVertexArray(mVao);
	glGenBuffers(1, &destVbo);
	glBindBuffer(GL_ARRAY_BUFFER, destVbo);
	glGenBuffers(1, &destEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, destEbo);


	glBufferData(GL_ARRAY_BUFFER, newVboSize, nullptr, GL_STATIC_DRAW);
	glCopyNamedBufferSubData(mVbo, destVbo, 0, 0, mVboSize);
	glDeleteBuffers(1, &mVbo);

	glBufferSubData(GL_ARRAY_BUFFER, mVboSize, rMesh.GetNumVertices() * mVertexSize, rMesh.GetInterleavedData());
	

	rMesh.SetVboPosition(mVboSize);
	mVbo = destVbo;
	mVboSize = newVboSize;

	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newEboSize, nullptr, GL_STATIC_DRAW);
	glCopyNamedBufferSubData(mEbo, destEbo, 0, 0, mEboSize);
	glDeleteBuffers(1, &mEbo);

	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, mEboSize, rMesh.GetNumIndices() * sizeof(unsigned int), rMesh.GetIndices());
	

	rMesh.SetEboPosition(mEboSize);
	mEbo = destEbo;
	mEboSize = newEboSize;


	unsigned int idx = 0;
	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);

		++idx;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void GeometryBatch::AddCommand(const Command& command)
{
	mCommands.push_back(command);
}


void GeometryBatch::Draw()
{
	App->GetOpenGL()->BindSceneFramebuffer();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());


	glBindVertexArray(mVao);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mCommands.size() * sizeof(Command), mCommands.data(), GL_STATIC_DRAW);
	

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(float) * 16, nullptr, GL_STATIC_DRAW);
	

	unsigned int offset = 0;
	for (const MeshRendererComponent* mesh : mMeshComponents) {

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(float) * 16, mesh->GetOwner()->GetWorldTransform().ptr());
		offset += sizeof(float) * 16;
	}


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(Material), nullptr, GL_STATIC_DRAW);

	offset = 0;
	for (const MeshRendererComponent* mesh : mMeshComponents) {

		const ResourceMaterial* rMaterial = mesh->GetMaterial();
		Material material;
		memcpy(material.diffuseColor, rMaterial->mDiffuseFactor.ptr(), sizeof(float) * 4);
		material.diffuseTexture = rMaterial->mDiffuseTexture->mTextureHandle;
		memcpy(material.specularColor, rMaterial->mSpecularFactor.ptr(), sizeof(float) * 4);
		material.specularTexture = rMaterial->mSpecularGlossinessTexture->mTextureHandle;
		material.normalTexture = rMaterial->mNormalTexture->mTextureHandle;
		material.shininess = rMaterial->mGlossinessFactor;
		material.hasDiffuseMap = rMaterial->mEnableDiffuseTexture;
		material.hasSpecularMap = rMaterial->mEnableSpecularGlossinessTexture;
		material.hasShininessMap = rMaterial->mEnableShinessMap;
		material.hasNormalMap = rMaterial->mEnableNormalMap;
		
		//mMaterials.push_back(material);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
		offset += sizeof(Material);

		//glMakeTextureHandleResidentARB(material.diffuseTexture);
		//glMakeTextureHandleResidentARB(material.specularTexture);
		//glMakeTextureHandleResidentARB(material.normalTexture);
	}

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0 , mCommands.size(), 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glUseProgram(0);
	glBindVertexArray(0);

	//for (const Material* material : mMaterials) {
	//	glMakeTextureHandleNonResidentARB(material->diffuseTexture);
	//	glMakeTextureHandleNonResidentARB(material->specularTexture);
	//	glMakeTextureHandleNonResidentARB(material->normalTexture);
	//}
	//
	//for (const Material* material : mMaterials) {
	//	delete material;
	//}
	//
	//mMaterials.clear();
	mCommands.clear();

	App->GetOpenGL()->UnbindSceneFramebuffer();
}

