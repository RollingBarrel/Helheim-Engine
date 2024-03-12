#include "Globals.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "GeometryBatch.h"
#include <cassert>
#include "glew.h"
#include "float3.h"
#include "float4x4.h"
#include "MeshRendererComponent.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

GeometryBatch::GeometryBatch(const MeshRendererComponent* cMesh)
{
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &mSsboAligment);
	
	memset(mSsboModelMatricesData, 0, sizeof(mSsboModelMatricesData));
	memset(mSsboIndicesData, 0, sizeof(mSsboIndicesData));
	memset(mSync, 0, sizeof(mSync));

	std::vector<Attribute> attributes;
	cMesh->GetResourceMesh()->GetAttributes(attributes);
	for (const Attribute attr : attributes)
	{
		mAttributes.push_back(attr);
	}
	mVertexSize = cMesh->GetResourceMesh()->GetVertexSize();

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glGenBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboMaterials);
	glGenBuffers(1, &mSsboIndices);
	glGenBuffers(1, &mIbo);
	
	unsigned int idx = 0;
	for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it).size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it).offset);
		++idx;
	}

	glBindVertexArray(0);
	AddMeshComponent(cMesh);
}

GeometryBatch::~GeometryBatch()
{
	delete[] mVboData;
	delete[] mEboData;
	glDeleteVertexArrays(1, &mVao);
	glDeleteBuffers(1, &mVbo);
	glDeleteBuffers(1, &mEbo);
	glDeleteBuffers(1, &mSsboModelMatrices);
	glDeleteBuffers(1, &mSsboMaterials);
	glDeleteBuffers(1, &mSsboIndices);
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
	//unsigned int offset = 0;
	//for (const MeshRendererComponent* mesh : mMeshComponents) {
	//
	//	if (mesh == cMesh)
	//	{
	//		//const ResourceMaterial* rMaterial = mesh->GetMaterial();
	//		//Material material;
	//		//memcpy(material.diffuseColor, rMaterial->mDiffuseFactor.ptr(), sizeof(float) * 4);
	//		//material.diffuseTexture = rMaterial->mDiffuseTexture->mTextureHandle;
	//		//memcpy(material.specularColor, rMaterial->mSpecularFactor.ptr(), sizeof(float) * 4);
	//		//material.specularTexture = rMaterial->mSpecularGlossinessTexture->mTextureHandle;
	//		//material.normalTexture = rMaterial->mNormalTexture->mTextureHandle;
	//		//material.shininess = rMaterial->mGlossinessFactor;
	//		//material.hasDiffuseMap = rMaterial->mEnableDiffuseTexture;
	//		//material.hasSpecularMap = rMaterial->mEnableSpecularGlossinessTexture;
	//		//material.hasShininessMap = rMaterial->mEnableShinessMap;
	//		//material.hasNormalMap = rMaterial->mEnableNormalMap;
	//		//
	//		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	//		//glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
	//		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//		break;
	//	}
	//	offset += sizeof(Material);
	//}
}

#define ALIGNED_STRUCT_SIZE(STRUCT_SIZE, ALIGNMENT) ((STRUCT_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
void GeometryBatch::RecreatePersistentSsbosAndIbo()
{
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mMeshComponents.size() * sizeof(Command), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModelMatrices);
	unsigned int size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboModelMatricesData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboModelMatricesData[i] = mSsboModelMatricesData[0] + ((size * i) / sizeof(float));
	}

	glDeleteBuffers(1, &mSsboIndices);
	glGenBuffers(1, &mSsboIndices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboIndices);
	size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboIndicesData[0] = reinterpret_cast<uint32_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboIndicesData[i] = mSsboIndicesData[0] + ((size * i) / sizeof(uint32_t));
	}
}

void GeometryBatch::AddMeshComponent(const MeshRendererComponent* cMesh)
{
	bool found = false;
	
	uint32_t matIdx = 0;
	const ResourceMaterial& resourceMaterial = *cMesh->GetResourceMaterial();
	for (const BatchMaterialResource material : mUniqueMaterials) {
		if (material.resource->GetUID() == resourceMaterial.GetUID()) {
			found = true;
			++mUniqueMaterials[matIdx].referenceCount;
			break;
		}
		matIdx++;
	}
	if (!found)
	{
		unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), mSsboAligment);
		mUniqueMaterials.emplace_back(&resourceMaterial);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
		unsigned int offset = 0;
		for (const BatchMaterialResource rMaterial : mUniqueMaterials) {
			Material material;
			memcpy(material.diffuseColor, rMaterial.resource->GetDiffuseFactor().ptr(), sizeof(float) * 4);
			
			material.diffuseTexture = (rMaterial.resource->GetDiffuseTexture()) ? rMaterial.resource->GetDiffuseTexture()->GetTextureHandle() : 0;
			memcpy(material.specularColor, rMaterial.resource->GetSpecularFactor().ptr(), sizeof(float) * 4);
			material.specularTexture = (rMaterial.resource->GetSpecularGlossinessTexture()) ? rMaterial.resource->GetSpecularGlossinessTexture()->GetTextureHandle() : 0;
			material.normalTexture = (rMaterial.resource->GetNormalTexture()) ? rMaterial.resource->GetNormalTexture()->GetTextureHandle() : 0;
			material.shininess = rMaterial.resource->GetGlossinessFactor();
			material.hasDiffuseMap = rMaterial.resource->IsDiffuseTextureEnabled();
			material.hasSpecularMap = rMaterial.resource->IsSpecularGlossinessTextureEnabled();
			material.hasShininessMap = rMaterial.resource->IsShininessMapEnabled();
			material.hasNormalMap = rMaterial.resource->IsNormalMapEnabled();

			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, materialSize, &material);
			offset += materialSize;
		}
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	found = false;
	const ResourceMesh& rMesh = *cMesh->GetResourceMesh();
	for (unsigned int i = 0; i < mUniqueMeshes.size(); ++i)
	{
		if (mUniqueMeshes[i].resource->GetUID() == rMesh.GetUID())
		{
			found = true;
			++mUniqueMeshes[i].referenceCount;
			break;
		}
	}
	if (!found)
	{
		unsigned int baseVertex = mVboNumElements * sizeof(float) / rMesh.GetVertexSize();
		unsigned int newNumElements = mVboNumElements + rMesh.GetNumberVertices() * rMesh.GetVertexSize() / sizeof(float);
		float* tmp = new float[newNumElements];
		if (mVboData)
			memcpy(tmp, mVboData, mVboNumElements * sizeof(float));
		float* newElements = rMesh.GetInterleavedData();
		memcpy(tmp + mVboNumElements, newElements, rMesh.GetNumberVertices() * rMesh.GetVertexSize());
		delete[] newElements;
		delete[] mVboData;
		mVboData = tmp;
		mVboNumElements = newNumElements;
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, mVboNumElements * sizeof(float), mVboData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		unsigned int firstIndex = mEboNumElements;
		newNumElements = mEboNumElements + rMesh.GetNumberIndices();
		unsigned int* tmp2 = new unsigned int[newNumElements];
		if (mEboData)
			memcpy(tmp2, mEboData, mEboNumElements * sizeof(unsigned int));
		memcpy(tmp2 + mEboNumElements, rMesh.GetIndices(), rMesh.GetNumberIndices() * sizeof(unsigned int));
		mEboNumElements = newNumElements;
		delete[] mEboData;
		mEboData = tmp2;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboNumElements * sizeof(unsigned int), mEboData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mUniqueMeshes.emplace_back(&rMesh, firstIndex, baseVertex);
	}


	mMeshComponents.emplace_back(cMesh, mUniqueMeshes.size() - 1, mUniqueMaterials.size() - 1);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, mMeshComponents.size() * sizeof(Command), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboModelMatrices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModelMatrices);
	unsigned int size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboModelMatricesData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboModelMatricesData[i] = mSsboModelMatricesData[0] + ((size * i) / sizeof(float));
	}

	glDeleteBuffers(1, &mSsboIndices);
	glGenBuffers(1, &mSsboIndices);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboIndices);
	size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), mSsboAligment);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboIndicesData[0] = reinterpret_cast<uint32_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboIndicesData[i] = mSsboIndicesData[0] + ((size * i) / sizeof(uint32_t));
	}
}

bool GeometryBatch::RemoveMeshComponent(const MeshRendererComponent* component)
{
	unsigned int bMeshIdx = 0;
	unsigned int bMaterialIdx = 0;
	bool found = false;
	const ResourceMesh& rMesh = *component->GetResourceMesh();
	for (std::vector<BatchMeshRendererComponent>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
	{
		if (it->component->GetID() == component->GetID())
		{
			bMeshIdx = it->bMeshIdx;
			bMaterialIdx = it->bMaterialIdx;
			mMeshComponents.erase(it);
			found = true;
			break;
		}
	}
	if (!found)
		return false;

	if (!--mUniqueMeshes[bMeshIdx].referenceCount)
	{
		unsigned int offset = mUniqueMeshes[bMeshIdx].baseVertex * rMesh.GetVertexSize() / sizeof(float);
		unsigned int size = rMesh.GetNumberVertices() * GetVertexSize();
		memmove(mVboData + offset, mVboData + (offset + size / sizeof(float)) - 1, size);
		mVboNumElements -= size / sizeof(float);
		float* tmp = new float[mVboNumElements];
		memcpy(tmp, mVboData, mVboNumElements * sizeof(float));
		delete[] mVboData;
		mVboData = tmp;
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, mVboNumElements * sizeof(float), mVboData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		memmove(mEboData + mUniqueMeshes[bMeshIdx].firstIndex, mEboData + mUniqueMeshes[bMeshIdx].firstIndex + rMesh.GetNumberIndices() - 1, rMesh.GetNumberIndices() * sizeof(unsigned int));
		mEboNumElements -= rMesh.GetNumberIndices();
		unsigned int* indicesTmp = new unsigned int[mEboNumElements];
		memcpy(indicesTmp, mEboData, mEboNumElements * sizeof(unsigned int));
		delete[] mEboData;
		mEboData = indicesTmp;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboNumElements, mEboData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		std::vector<BatchMeshResource>::iterator its = mUniqueMeshes.erase(mUniqueMeshes.begin() + bMeshIdx);
		for (std::vector<BatchMeshResource>::iterator it = its; it != mUniqueMeshes.end(); ++it)
		{
			it->baseVertex -= rMesh.GetNumberVertices();
			it->firstIndex -= rMesh.GetNumberIndices();
		}
		for (std::vector<BatchMeshRendererComponent>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->bMeshIdx > bMeshIdx)
				--(it->bMeshIdx);
		}
	}
	if (!--mUniqueMaterials[bMaterialIdx].referenceCount)
	{
		unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), mSsboAligment);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
		unsigned int offset = bMaterialIdx * materialSize;
		std::vector<BatchMaterialResource>::iterator its = mUniqueMaterials.erase(mUniqueMaterials.begin() + bMaterialIdx);
		for (std::vector<BatchMaterialResource>::iterator rMaterial = its; rMaterial != mUniqueMaterials.end(); ++rMaterial)
		{
			if ((rMaterial + 1) != mUniqueMaterials.end())
			{
				Material material;
				memcpy(material.diffuseColor, rMaterial->resource->GetDiffuseFactor().ptr(), sizeof(material.diffuseColor));

				material.diffuseTexture = (rMaterial->resource->GetDiffuseTexture()) ? rMaterial->resource->GetDiffuseTexture()->GetTextureHandle() : 0;
				memcpy(material.specularColor, rMaterial->resource->GetSpecularFactor().ptr(), sizeof(float) * 4);
				material.specularTexture = (rMaterial->resource->GetSpecularGlossinessTexture()) ? rMaterial->resource->GetSpecularGlossinessTexture()->GetTextureHandle() : 0;
				material.normalTexture = (rMaterial->resource->GetNormalTexture()) ? rMaterial->resource->GetNormalTexture()->GetTextureHandle() : 0;
				material.shininess = rMaterial->resource->GetGlossinessFactor();
				material.hasDiffuseMap = rMaterial->resource->IsDiffuseTextureEnabled();
				material.hasSpecularMap = rMaterial->resource->IsSpecularGlossinessTextureEnabled();
				material.hasShininessMap = rMaterial->resource->IsShininessMapEnabled();
				material.hasNormalMap = rMaterial->resource->IsNormalMapEnabled();

				glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, materialSize, &material);
				offset += materialSize;
			}
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		for (std::vector<BatchMeshRendererComponent>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->bMaterialIdx > bMaterialIdx)
				--(it->bMaterialIdx);
		}
	}

	return true;
}

void GeometryBatch::Draw()
{
	if (mMeshComponents.size() == 0)
		return;
	glUseProgram(App->GetOpenGL()->GetPBRProgramId());
	glBindVertexArray(mVao);

	unsigned int idx = mDrawCount % NUM_BUFFERS;
	if (mSync[idx])
	{
		GLenum waitReturn = GL_UNSIGNALED;
		while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED) 
		{
			waitReturn = glClientWaitSync(mSync[idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
		}
	}

	mCommands.clear();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModelMatrices);
	unsigned int i = 0;
	for (const BatchMeshRendererComponent batchMeshRenderer : mMeshComponents)
	{
		const MeshRendererComponent* meshRenderer = batchMeshRenderer.component;
		const ResourceMesh* rMesh = meshRenderer->GetResourceMesh();
		// Enable/disable mesh renderer component
		if (meshRenderer->IsEnabled() && meshRenderer->GetOwner()->IsActive())
		{
			if (!App->GetScene()->GetApplyFrustumCulling() || meshRenderer->IsInsideFrustum())
			{
				memcpy(mSsboModelMatricesData[idx] + 16 * i, meshRenderer->GetOwner()->GetWorldTransform().ptr(), sizeof(float) * 16);
				memcpy(mSsboIndicesData[idx] + i, &batchMeshRenderer.bMaterialIdx, sizeof(uint32_t));
				mCommands.emplace_back(rMesh->GetNumberIndices(), 1, mUniqueMeshes[batchMeshRenderer.bMeshIdx].firstIndex, mUniqueMeshes[batchMeshRenderer.bMeshIdx].baseVertex, mCommands.size());
			}
		}
		++i;
	}

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIbo);
	glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, mCommands.size() * sizeof(Command), mCommands.data());

	unsigned int structSize = ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, mSsboModelMatrices, idx * mMeshComponents.size() * structSize, mMeshComponents.size() * structSize);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	structSize = ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), mSsboAligment);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 12, mSsboIndices, idx * mMeshComponents.size() * structSize, mMeshComponents.size() * structSize);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0 , mCommands.size(), 0);
	
	glDeleteSync(mSync[idx]);
	mSync[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	

	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glUseProgram(0);
	mCommands.clear();

	++mDrawCount;
}

