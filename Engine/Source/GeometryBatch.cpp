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
#include "AnimationComponent.h"

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

	glGenBuffers(1, &mPaletteSsbo);
	glGenBuffers(1, &mBoneIndicesSsbo);
	glGenBuffers(1, &mWeightsSsbo);
	glGenBuffers(1, &mPosSsbo);
	glGenBuffers(1, &mNormSsbo);
	glGenBuffers(1, &mTangSsbo);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mPaletteSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 16, mBoneIndicesSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 17, mWeightsSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 18, mPosSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 19, mNormSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 20, mTangSsbo);

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

	glDeleteBuffers(1, &mPaletteSsbo);
	glDeleteBuffers(1, &mBoneIndicesSsbo);
	glDeleteBuffers(1, &mWeightsSsbo);
	glDeleteBuffers(1, &mPosSsbo);
	glDeleteBuffers(1, &mNormSsbo);
	glDeleteBuffers(1, &mTangSsbo);

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

#define ALIGNED_STRUCT_SIZE(STRUCT_SIZE, ALIGNMENT) ((STRUCT_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
bool GeometryBatch::EditMaterial(const MeshRendererComponent* cMesh)
{
	const ResourceMaterial* rMat = cMesh->GetResourceMaterial();
	unsigned int offset = 0;
	int idx = 0;
	unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), sizeof(float)*3);
	for (BatchMaterialResource bRMaterial : mUniqueMaterials)
	{
		if (rMat->GetUID() == bRMaterial.resource->GetUID())
		{
			break;
		}
		++idx;
		offset += materialSize;
	}
	if (mUniqueMaterials.size() == 0 || idx == mUniqueMaterials.size())
		return false;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	const BatchMaterialResource& rMaterial = mUniqueMaterials[idx];
	Material material;
	memcpy(material.baseColor, rMaterial.resource->GetBaseColorFactor().ptr(), sizeof(float) * 3);
	material.baseColorTex = (rMaterial.resource->GetBaseColorTexture()) ? rMaterial.resource->GetBaseColorTexture()->GetTextureHandle() : 0;
	material.metalRoughTex = (rMaterial.resource->GetMetallicRoughnessTexture()) ? rMaterial.resource->GetMetallicRoughnessTexture()->GetTextureHandle() : 0;
	material.normalTex = (rMaterial.resource->GetNormalTexture()) ? rMaterial.resource->GetNormalTexture()->GetTextureHandle() : 0;
	material.hasMetalRoughTex = rMaterial.resource->IsMetallicRoughnessEnabled();
	material.hasBaseColorTex = rMaterial.resource->IsBaseColorEnabled();
	material.hasNormalMap = rMaterial.resource->IsNormalMapEnabled();
	material.metalness = rMaterial.resource->GetMetallicFactor();
	material.roughness = rMaterial.resource->GetRoughnessFactor();

	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return true;
}

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

	mPersistentsFlag = false;
}

void GeometryBatch::RecreateVboAndEbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glBufferData(GL_ARRAY_BUFFER, mVboDataSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboDataSize, nullptr, GL_STATIC_DRAW);
	unsigned int vboOffset = 0;
	unsigned int eboOffset = 0;
	for (const BatchMeshResource& res : mUniqueMeshes)
	{
		unsigned int size = res.resource->GetNumberVertices() * res.resource->GetVertexSize();
		float* data = res.resource->GetInterleavedData();
		glBufferSubData(GL_ARRAY_BUFFER, vboOffset, size, data);
		vboOffset += size;
		delete[] data;
		size = res.resource->GetNumberIndices() * sizeof(unsigned int);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, eboOffset, size, res.resource->GetIndices());
		eboOffset += size;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mVBOFlag = false;
}

void GeometryBatch::RecreateMaterials()
{
	unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), sizeof(float)*3);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
	unsigned int offset = 0;
	for (const BatchMaterialResource rMaterial : mUniqueMaterials) {
		Material material;
		memcpy(material.baseColor, rMaterial.resource->GetBaseColorFactor().ptr(), sizeof(float) * 3);
		material.baseColorTex = (rMaterial.resource->GetBaseColorTexture()) ? rMaterial.resource->GetBaseColorTexture()->GetTextureHandle() : 0;
		material.metalRoughTex = (rMaterial.resource->GetMetallicRoughnessTexture()) ? rMaterial.resource->GetMetallicRoughnessTexture()->GetTextureHandle() : 0;
		material.normalTex = (rMaterial.resource->GetNormalTexture()) ? rMaterial.resource->GetNormalTexture()->GetTextureHandle() : 0;
		material.hasMetalRoughTex = rMaterial.resource->IsMetallicRoughnessEnabled();
		material.hasBaseColorTex = rMaterial.resource->IsBaseColorEnabled();
		material.hasNormalMap = rMaterial.resource->IsNormalMapEnabled();
		material.metalness = rMaterial.resource->GetMetallicFactor();
		material.roughness = rMaterial.resource->GetRoughnessFactor();

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
		offset += materialSize;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	mMaterialFlag = false;
}

void GeometryBatch::AddMeshComponent(const MeshRendererComponent* cMesh)
{
	bool foundMaterial = false;
	uint32_t matIdx = 0;
	const ResourceMaterial& resourceMaterial = *cMesh->GetResourceMaterial();
	for (const BatchMaterialResource material : mUniqueMaterials) {
		if (material.resource->GetUID() == resourceMaterial.GetUID()) {
			foundMaterial = true;
			++mUniqueMaterials[matIdx].referenceCount;
			break;
		}
		++matIdx;
	}
	if (!foundMaterial)
	{
        mUniqueMaterials.emplace_back(&resourceMaterial);
		matIdx = mUniqueMaterials.size() - 1;
		mMaterialFlag = true;  
	}

    bool foundMesh = false;  
	const ResourceMesh& rMesh = *cMesh->GetResourceMesh();                                                              
	unsigned int meshIdx = 0;
	for (; meshIdx < mUniqueMeshes.size(); ++meshIdx)
	{
		if (mUniqueMeshes[meshIdx].resource->GetUID() == rMesh.GetUID())
		{
			foundMesh = true;
			++mUniqueMeshes[meshIdx].referenceCount;
			break;
		}
	}
	if (!foundMesh)
	{
		mUniqueMeshes.emplace_back(&rMesh, mEboDataSize / sizeof(unsigned int), mVboDataSize / rMesh.GetVertexSize());
		meshIdx = mUniqueMeshes.size() - 1;
		mVBOFlag = true;
		mVboDataSize += rMesh.GetNumberVertices() * rMesh.GetVertexSize();
		mEboDataSize += rMesh.GetNumberIndices() * sizeof(unsigned int);
	}

	mMeshComponents.emplace_back(cMesh, meshIdx, matIdx);
	mPersistentsFlag = true;
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
		mVboDataSize -= rMesh.GetNumberVertices() * rMesh.GetVertexSize();
		mEboDataSize -= rMesh.GetNumberIndices() * sizeof(unsigned int);
		mVBOFlag = true;
	}
	if (!--mUniqueMaterials[bMaterialIdx].referenceCount)
	{
		mUniqueMaterials.erase(mUniqueMaterials.begin() + bMaterialIdx);
		mMaterialFlag = true;
		for (std::vector<BatchMeshRendererComponent>::iterator it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->bMaterialIdx > bMaterialIdx)
			{
				--(it->bMaterialIdx);
			}
		}
	}
	
	mPersistentsFlag = true;
	return true;
}

void GeometryBatch::Draw()
{
	if (mMeshComponents.size() == 0)
		return;

	if (mVBOFlag)
		RecreateVboAndEbo();
	if (mMaterialFlag)
		RecreateMaterials();
	if (mPersistentsFlag)
		RecreatePersistentSsbosAndIbo();

	bool animationSkinning = false;
	glUseProgram(App->GetOpenGL()->GetSkinningProgramId());
	for (const BatchMeshRendererComponent batchMeshRenderer : mMeshComponents)
	{
		const MeshRendererComponent* meshRenderer = batchMeshRenderer.component;
		const ResourceMesh* rMesh = meshRenderer->GetResourceMesh();
		if (meshRenderer->IsEnabled() && meshRenderer->GetOwner()->IsActive())
		{
			if (meshRenderer->GetIsAnimated() != 0 && (!App->GetScene()->GetApplyFrustumCulling() || meshRenderer->IsInsideFrustum()))
			{
				const AnimationComponent* cAnim = meshRenderer->GetAnimationComponent();
				if (cAnim && cAnim->GetIsPlaying())
				{
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPaletteSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, meshRenderer->GetPalette().size() * sizeof(float) * 16, meshRenderer->GetPalette().data(), GL_DYNAMIC_DRAW);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mBoneIndicesSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberJoints() * sizeof(unsigned int), rMesh->GetJoints(), GL_STREAM_DRAW);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mWeightsSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberWeights() * sizeof(float), rMesh->GetWeights(), GL_STREAM_DRAW);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPosSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberVertices() * sizeof(float) * 3, rMesh->GetAttributeData(Attribute::POS), GL_STREAM_DRAW);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mNormSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberVertices() * sizeof(float) * 3, rMesh->GetAttributeData(Attribute::NORMAL), GL_STREAM_DRAW);
					glBindBuffer(GL_SHADER_STORAGE_BUFFER, mTangSsbo);
					glBufferData(GL_SHADER_STORAGE_BUFFER, rMesh->GetNumberVertices() * sizeof(float) * 4, rMesh->GetAttributeData(Attribute::TANGENT), GL_STREAM_DRAW);
					glUniform1i(25, rMesh->GetNumberVertices());
					glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 21, mVbo, mUniqueMeshes[batchMeshRenderer.bMeshIdx].baseVertex * rMesh->GetVertexSize(), rMesh->GetVertexSize() * rMesh->GetNumberVertices());
					glDispatchCompute((rMesh->GetNumberVertices() + (63)) / 64, 1, 1);
					animationSkinning = true;
				}
			}
		}
	}
	if(animationSkinning)
		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

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
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, mCommands.size(), 0);

	glDeleteSync(mSync[idx]);
	mSync[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);


	glBindVertexArray(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glUseProgram(0);
	mCommands.clear();

	++mDrawCount;
}

