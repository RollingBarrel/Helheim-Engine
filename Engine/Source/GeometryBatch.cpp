#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include "float3.h"
#include "float4x4.h"
#include "MeshRendererComponent.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"

GeometryBatch::GeometryBatch(MeshRendererComponent* mesh)
{
	memset(mSsboModelsData, 0, sizeof(mSsboModelsData)* NUM_BUFFERS);
	memset(mSync, 0, sizeof(mSync)* NUM_BUFFERS);

	const std::vector<Attribute*>& attributes = mesh->GetResourceMesh()->GetAttributes();
	for (const Attribute* ptrAttr : attributes)
	{
		mAttributes.push_back(*ptrAttr);
	}
	mVertexSize = mesh->GetResourceMesh()->GetVertexSize();

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glGenBuffers(1, &mSsboModels);
	glGenBuffers(1, &mSsboMaterials);;
	glGenBuffers(1, &mIbo);
	

	unsigned int idx = 0;
	for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it).size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it).offset);
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
	mMeshComponents.clear();
	mUniqueMeshes.clear();
	mAttributes.clear();
	mCommands.clear();
}

void GeometryBatch::GetAttributes(std::vector<Attribute>& attributes) const
{
	for (Attribute attribute : mAttributes)
	{
		attributes.push_back(attribute);
	}
}

void GeometryBatch::EditMaterial(const MeshRendererComponent* cMesh)
{
	unsigned int offset = 0;
	for (const MeshRendererComponent* mesh : mMeshComponents) {

		if (mesh == cMesh)
		{
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

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			break;
		}
		offset += sizeof(Material);
	}
}

void GeometryBatch::AddMesh(const MeshRendererComponent* cMesh)
{
	mMeshComponents.push_back(cMesh);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mMeshComponents.size() * sizeof(Command), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mSsboModels);
	glGenBuffers(1, &mSsboModels);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	unsigned int size = mMeshComponents.size() * sizeof(float) * 16;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboModelsData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboModelsData[i] = mSsboModelsData[0] + ((size * i) / sizeof(float));
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(Material), nullptr, GL_STATIC_DRAW);

	unsigned int offset = 0;
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

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
		offset += sizeof(Material);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


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
		if ((*it)->GetResourceMesh()->mUID == rMesh.mUID)
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
		//if(found >= 1)
		//{
		//	ResourceMesh* rMesh2 = const_cast<ResourceMesh*>((*it)->GetResourceMesh());
		//	rMesh2->SetEboPosition(rMesh2->GetEboPosition() - rMesh.GetNumIndices());
		//	int a = (rMesh.GetNumVertices() * (rMesh.GetVertexSize() / sizeof(float)));
		//	rMesh2->SetVertexBase(rMesh2->GetVertexBase() - a);
		//}
		++it;
	}
	if (found == 1)
	{
		for (std::vector<const ResourceMesh*>::iterator it = mUniqueMeshes.begin(); it != mUniqueMeshes.end(); ++it)
		{
			if (&rMesh == *it)
			{
				mUniqueMeshes.erase(it);
				break;
			}
		}
	}
}


void GeometryBatch::AddCommand(const MeshRendererComponent* mesh)
{
	
	Command command;
	command.mCount = mesh->GetResourceMesh()->GetNumIndices();
	command.mInstanceCount = 1;
	command.firstIndex = mesh->GetResourceMesh()->GetEboPosition();
	command.baseVertex = mesh->GetResourceMesh()->GetVertexBase();
	command.baseInstance = mCommands.size();

	mCommands.push_back(command);
}


void GeometryBatch::Draw()
{
	App->GetOpenGL()->BindSceneFramebuffer();

	glUseProgram(App->GetOpenGL()->GetPBRProgramId());
	glBindVertexArray(mVao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, mCommands.size() * sizeof(Command), mCommands.data());

	unsigned int idx = mDrawCount % NUM_BUFFERS;
	if (mSync[idx])
	{
		GLenum waitReturn = GL_UNSIGNALED;
		while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED) {
			waitReturn = glClientWaitSync(mSync[idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModels);
	for (unsigned int i = 0; i < mMeshComponents.size(); ++i) {
		memcpy(mSsboModelsData[idx] + 16 * i, mMeshComponents[i]->GetOwner()->GetWorldTransform().ptr(), sizeof(float) * 16);
	}

	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, mSsboModels, 0, mMeshComponents.size() * sizeof(float) * 16);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0 , mCommands.size(), 0);
	
	glDeleteSync(mSync[idx]);
	mSync[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	

	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glUseProgram(0);
	mCommands.clear();
	App->GetOpenGL()->UnbindSceneFramebuffer();

	++mDrawCount;
}

