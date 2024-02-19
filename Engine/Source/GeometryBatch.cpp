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
	mAttributes = mesh->GetResourceMesh()->GetAttributes();
	mVertexSize = mesh->GetResourceMesh()->GetVertexSize();

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glGenBuffers(1, &mSsboModels);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels);
	glGenBuffers(1, &mSsboMaterials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glGenBuffers(1, &mIbo);

	unsigned int idx = 0;
	for (std::vector<Attribute*>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it)->size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it)->offset);
		
		++idx;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	AddMesh(mesh);

}

GeometryBatch::~GeometryBatch()
{
	delete[] mVboData;
	delete[] mEboData;
	glDeleteVertexArrays(1, &mVao);
	glDeleteBuffers(1, &mVbo);
	glDeleteBuffers(1, &mEbo);
	glDeleteBuffers(1, &mSsboModels);
	glDeleteBuffers(1, &mSsboMaterials);
	glDeleteBuffers(1, &mIbo);
}


void GeometryBatch::AddMesh(MeshRendererComponent* cMesh)
{
	mMeshComponents.push_back(cMesh);

	ResourceMesh& rMesh = const_cast<ResourceMesh&>(*cMesh->GetResourceMesh());
	for (const ResourceMesh* mesh : mUniqueMeshes) {
		if (mesh->mUID == rMesh.mUID) {
			rMesh.SetVertexBase(mesh->GetVertexBase());
			rMesh.SetEboPosition(mesh->GetEboPosition());
			return;
		}
	}

	mUniqueMeshes.push_back(&rMesh);

	rMesh.SetVertexBase(mVboNumElements * sizeof(float) / rMesh.GetVertexSize());
	unsigned int newNumElements = mVboNumElements + rMesh.GetNumVertices() * rMesh.GetVertexSize() / sizeof(float);
	float* tmp = new float[newNumElements];
	if(mVboData)
		memcpy(tmp, mVboData, mVboNumElements * sizeof(float));
	float* newElements = rMesh.GetInterleavedData();
	memcpy(tmp + mVboNumElements, newElements, rMesh.GetNumVertices() * rMesh.GetVertexSize());
	delete[] newElements;
	delete[] mVboData;
	mVboData = tmp;
	mVboNumElements = newNumElements;
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboNumElements * sizeof(float), mVboData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	rMesh.SetEboPosition(mEboNumElements);
	newNumElements = mEboNumElements + rMesh.GetNumIndices();
	unsigned int* tmp2 = new unsigned int[newNumElements];
	if (mEboData)
		memcpy(tmp2, mEboData, mEboNumElements * sizeof(unsigned int));
	memcpy(tmp2 + mEboNumElements, rMesh.GetIndices(), rMesh.GetNumIndices() * sizeof(unsigned int));
	mEboNumElements = newNumElements;
	delete[] mEboData;
	mEboData = tmp2;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboNumElements * sizeof(unsigned int), mEboData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void GeometryBatch::RemoveMesh(const MeshRendererComponent* component)
{
	const ResourceMesh& rMesh = *component->GetResourceMesh();
	unsigned int found = 0;
	for (std::vector<const MeshRendererComponent*>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end();)
	{
		if (component->GetResourceMesh()->mUID == rMesh.mUID)
		{
			++found;
		}
		if (component == *it)
		{
			unsigned int offset = rMesh.GetVertexBase() * rMesh.GetVertexSize() / sizeof(float);
			unsigned int size = rMesh.GetNumVertices() * GetVertexSize();
			memmove(mVboData + offset, mVboData + offset + size / sizeof(float), size);
			mVboNumElements -= size / sizeof(float);
			float* tmp = new float[mVboNumElements];
			delete[] mVboData;
			mVboData = tmp;
			glBindBuffer(GL_ARRAY_BUFFER, mVbo);
			glBufferData(GL_ARRAY_BUFFER, mVboNumElements * sizeof(float), mVboData, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			memmove(mEboData + rMesh.GetEboPosition(), mEboData + rMesh.GetEboPosition() + rMesh.GetNumIndices(), rMesh.GetNumIndices() * sizeof(unsigned int));
			mEboNumElements -= rMesh.GetNumIndices();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboNumElements, mEboData, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			it = mMeshComponents.erase(it);
			continue;
		}
		++it;
	}
	if (found == 1)
	{
		for (std::vector<const ResourceMesh*>::iterator it = mUniqueMeshes.begin(); it != mUniqueMeshes.end(); ++it)
		{
			if (&rMesh == *it)
			{
				mUniqueMeshes.erase(it);
			}
		}
	}
}


void GeometryBatch::AddCommand(const MeshRendererComponent* mesh)
{
	
	Command command;
	command.mCount = mesh->GetResourceMesh()->GetNumIndices();
	command.mInstanceCount = 1;
	command.firstIndex = mesh->GetResourceMesh()->GetEboPosition();// / sizeof(GLuint);
	command.baseVertex = mesh->GetResourceMesh()->GetVboPosition();// / mBatch->GetVertexSize();
	command.baseInstance = mCommands.size();

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
	}

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0 , mCommands.size(), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	glUseProgram(0);
	mCommands.clear();
	App->GetOpenGL()->UnbindSceneFramebuffer();
}

