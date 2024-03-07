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
	
	memset(mSsboModelMatricesData, 0, sizeof(mSsboModelMatricesData)* NUM_BUFFERS);
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
	AddMesh(cMesh);
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

#define ALIGNED_STRUCT_SIZE(STRUCT_SIZE, ALIGNMENT) ((STRUCT_SIZE) + (ALIGNMENT - ((STRUCT_SIZE) % ALIGNMENT)))
void GeometryBatch::AddMesh(const MeshRendererComponent* cMesh)
{
	mMeshComponents.emplace_back(cMesh);

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

	uint32_t matIdx = 0;
	bool found = false;
	const ResourceMaterial& resourceMaterial = *cMesh->GetResourceMaterial();
	for (const ResourceMaterial* material : mUniqueMaterials) {
		if (material->GetUID() == resourceMaterial.GetUID()) {
			found = true;
			mMeshComponents.back().materialIdx = matIdx;
			break;
		}
		matIdx++;
	}
	if (!found)
	{
		unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), mSsboAligment);
		mMeshComponents.back().materialIdx = mUniqueMaterials.size();
		mUniqueMaterials.push_back(&resourceMaterial);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
		glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
		unsigned int offset = 0;
		for (const ResourceMaterial* rMaterial : mUniqueMaterials) {
			Material material;
			memcpy(material.diffuseColor, rMaterial->GetDiffuseFactor().ptr(), sizeof(float) * 4);
			
			material.diffuseTexture = (rMaterial->GetDiffuseTexture()) ? rMaterial->GetDiffuseTexture()->GetTextureHandle() : 0;
			memcpy(material.specularColor, rMaterial->GetSpecularFactor().ptr(), sizeof(float) * 4);
			material.specularTexture = (rMaterial->GetSpecularGlossinessTexture()) ? rMaterial->GetSpecularGlossinessTexture()->GetTextureHandle() : 0;
			material.normalTexture = (rMaterial->GetNormalTexture()) ? rMaterial->GetNormalTexture()->GetTextureHandle() : 0;
			material.shininess = rMaterial->GetGlossinessFactor();
			material.hasDiffuseMap = rMaterial->IsDiffuseTextureEnabled();
			material.hasSpecularMap = rMaterial->IsSpecularGlossinessTextureEnabled();
			material.hasShininessMap = rMaterial->IsShininessMapEnabled();
			material.hasNormalMap = rMaterial->IsNormalMapEnabled();

			glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, materialSize, &material);
			offset += materialSize;
		}
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	const ResourceMesh& rMesh = *cMesh->GetResourceMesh();
	for (unsigned int i = 0; i < mUniqueMeshes.size(); ++i)
	{
		if (mUniqueMeshes[i].resource->GetUID() == rMesh.GetUID())
		{
			mMeshComponents.back().resource = i;
			return;
		}
	}

	unsigned int baseVertex = mVboNumElements * sizeof(float) / rMesh.GetVertexSize();
	unsigned int newNumElements = mVboNumElements + rMesh.GetNumberVertices() * rMesh.GetVertexSize() / sizeof(float);
	float* tmp = new float[newNumElements];
	if(mVboData)
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
	mMeshComponents.back().resource = mUniqueMeshes.size() - 1;
}

void GeometryBatch::RemoveMesh(const MeshRendererComponent* component)
{
	const ResourceMesh& rMesh = *component->GetResourceMesh();
	unsigned int timesFound = 0;
	unsigned int numRemovedIndices = 0;
	unsigned int numRemovedVertices = 0;
	for (std::vector<BatchMeshRendererComponent>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end();)
	{
		if (it->component->GetResourceMesh()->GetUID() == rMesh.GetUID())
		{
			++timesFound;
			if (timesFound == 2)
				break;
			numRemovedVertices = rMesh.GetNumberVertices() * GetVertexSize() / sizeof(float);
			numRemovedIndices = rMesh.GetNumberIndices();
			it = mMeshComponents.erase(it);
			continue;
		}
		++it;
	}
	if (timesFound == 1)
	{
		bool found = false;
		for (std::vector<BatchMeshRendererResource>::iterator it = mUniqueMeshes.begin(); it != mUniqueMeshes.end();)
		{
			if (&rMesh == it->resource)
			{
				unsigned int offset = it->baseVertex * rMesh.GetVertexSize() / sizeof(float);
				unsigned int size = rMesh.GetNumberVertices() * GetVertexSize();
				memmove(mVboData + offset, mVboData + offset + size / sizeof(float), size);
				mVboNumElements -= size / sizeof(float);
				float* tmp = new float[mVboNumElements];
				delete[] mVboData;
				mVboData = tmp;
				glBindBuffer(GL_ARRAY_BUFFER, mVbo);
				glBufferData(GL_ARRAY_BUFFER, mVboNumElements * sizeof(float), mVboData, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				numRemovedVertices = size / sizeof(float);
	
				memmove(mEboData + it->firstIndex, mEboData + it->firstIndex + rMesh.GetNumberIndices(), rMesh.GetNumberIndices() * sizeof(unsigned int));
				mEboNumElements -= rMesh.GetNumberIndices();
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboNumElements, mEboData, GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				it = mUniqueMeshes.erase(it);
				numRemovedIndices = rMesh.GetNumberIndices();
				found = true;
				continue;
			}
			if (found)
			{
				it->baseVertex -= numRemovedVertices;
				it->firstIndex -= numRemovedIndices;
			}
			++it;
		}
	}
	//Materials
}

void GeometryBatch::Draw()
{
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
				memcpy(mSsboIndicesData[idx] + i, &batchMeshRenderer.materialIdx, sizeof(uint32_t));
				mCommands.emplace_back(rMesh->GetNumberIndices(), 1, mUniqueMeshes[batchMeshRenderer.resource].firstIndex, mUniqueMeshes[batchMeshRenderer.resource].baseVertex, mCommands.size());
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

