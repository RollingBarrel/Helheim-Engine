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
#include "Geometry/Frustum.h"
#include "Geometry/AABB.h"

GeometryBatch::GeometryBatch(const MeshRendererComponent& cMesh)
{
	glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &mSsboAligment);

	for (int i = 0; i < NUM_BUFFERS; ++i)
	{
		mSsboModelMatricesData[i] = nullptr;
	}
	memset(mSync, 0, sizeof(mSync)*NUM_BUFFERS);

	cMesh.GetResourceMesh()->GetAttributes(mAttributes, Attribute::Usage::RENDER);
	mVertexSize = cMesh.GetResourceMesh()->GetVertexSize(Attribute::Usage::RENDER);

	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	glGenBuffers(1, &mEbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	glGenBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboMaterials);
	glGenBuffers(1, &mSsboIndicesCommands);
	glGenBuffers(1, &mPaletteSsbo);
	glGenBuffers(1, &mSkinSsbo);
	glGenBuffers(1, &mFrustumsSsbo);
	glGenBuffers(1, &mSkinSsboObbs);
	glGenBuffers(1, &mSkinDispatchIndirectBuffer);

	glGenBuffers(1, &mParameterBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParameterBuffer);
	unsigned int startValue = 0;
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(startValue), &startValue, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 15, mPaletteSsbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 20, mSkinSsbo);

	for (std::vector<Attribute>::const_iterator it = mAttributes.cbegin(); it != mAttributes.cend(); ++it)
	{
		if (!(it->usage & Attribute::Usage::RENDER))
			continue;
		unsigned int idx = 0;
		switch (it->type)
		{
		case Attribute::POS:
			idx = 0;
			break;
		case Attribute::UV:
			idx = 1;
			break;
		case Attribute::NORMAL:
			idx = 2;
			break;
		case Attribute::TANGENT:
			idx = 3;
			break;
		case Attribute::JOINT:
		case Attribute::WEIGHT:
			break;
		default:
			assert(false && "Unsuported Vertex Attribute");
			break;
		}

		glEnableVertexAttribArray(idx);
		glVertexAttribPointer(idx, (*it).size / sizeof(float), GL_FLOAT, GL_FALSE, mVertexSize, (void*)(*it).offset); 
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
	glDeleteBuffers(1, &mSsboIndicesCommands);
	glDeleteBuffers(1, &mFrustumsSsbo);
	glDeleteBuffers(1, &mParameterBuffer);
	glDeleteBuffers(1, &mPaletteSsbo);
	glDeleteBuffers(1, &mSkinSsbo);
	glDeleteBuffers(1, &mSkinSsboObbs);
	glDeleteBuffers(1, &mSkinDispatchIndirectBuffer);
}

void GeometryBatch::GetAttributes(std::vector<Attribute>& attributes) const
{
	for (Attribute attribute : mAttributes)
	{
		if(attribute.usage & Attribute::Usage::RENDER)
			attributes.push_back(attribute);
	}
}

#define ALIGNED_STRUCT_SIZE(STRUCT_SIZE, ALIGNMENT) ((STRUCT_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
bool GeometryBatch::EditMaterial(const MeshRendererComponent& cMesh)
{
	const ResourceMaterial* rMat = cMesh.GetResourceMaterial();
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

	mMaterialFlag = true;

	return true;
}

void GeometryBatch::RecreatePersistentFrustums()
{
	mFrustumsSsboCapacity += mSsboAligment;
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mFrustumsSsbo);
	glGenBuffers(1, &mFrustumsSsbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mFrustumsSsbo);
	const unsigned int size = sizeof(float)*24 * mFrustumsSsboCapacity;
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
	mSsboFrustumsData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
	for (int i = 1; i < NUM_BUFFERS; ++i)
	{
		mSsboFrustumsData[i] = mSsboFrustumsData[0] + ((size * i) / sizeof(float));
	}
}

void GeometryBatch::RecreatePersistentSsbos()
{
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glDeleteBuffers(1, &mSsboModelMatrices);
	glGenBuffers(1, &mSsboModelMatrices);
	if (mMeshComponents.size())
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboModelMatrices);
		unsigned int size = mMeshComponents.size() * ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
		mSsboModelMatricesData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
		for (unsigned int i = 1; i < NUM_BUFFERS; ++i)
		{
			mSsboModelMatricesData[i] = mSsboModelMatricesData[0] + ((size * i) / sizeof(float));
		}
	}

	glDeleteBuffers(1, &mSsboIndicesCommands);
	glGenBuffers(1, &mSsboIndicesCommands);
	if (mMeshComponents.size())
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboIndicesCommands);
		unsigned int size = mMeshComponents.size() * sizeof(uint32_t);
		unsigned int inBetweenOffset = ALIGNED_STRUCT_SIZE(size, mSsboAligment) - size;
		size += inBetweenOffset;
		size += sizeof(Command) * mMeshComponents.size();
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, nullptr, flags);
		mSsboMatIndicesCommandsData = reinterpret_cast<uint32_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, flags));

		unsigned int offset = mMeshComponents.size() + ((inBetweenOffset + sizeof(uint32_t) - 1) / sizeof(uint32_t));
		for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
		{
			BatchMeshRendererComponent& bMesh = mMeshComponents[i];
			bMesh.baseInstance = i;
			mSsboMatIndicesCommandsData[i] = bMesh.bMaterialIdx;
			mSsboMatIndicesCommandsData[offset + i * 5] = mUniqueMeshes[bMesh.bMeshIdx].resource->GetNumberIndices();//Count: Number of indices in the mesh
			mSsboMatIndicesCommandsData[offset + i * 5 + 1] = 1; //InstanceCount: Number of instances to render
			mSsboMatIndicesCommandsData[offset + i * 5 + 2] = mUniqueMeshes[bMesh.bMeshIdx].firstIndex;	 //FirstIndex: Index offset in the EBO
			mSsboMatIndicesCommandsData[offset + i * 5 + 3] = mUniqueMeshes[bMesh.bMeshIdx].baseVertex;	 //BaseVertex: Vertex offset in the VBO
			mSsboMatIndicesCommandsData[offset + i * 5 + 4] = i;   //BaseInstance Instance Index
		}
	}

	glDeleteBuffers(1, &mSsboObbs);
	glGenBuffers(1, &mSsboObbs);
	if (mMeshComponents.size())
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboObbs);
		unsigned int size = mMeshComponents.size() * sizeof(float) * 32;
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
		mSsboObbsData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
		for (int i = 1; i < NUM_BUFFERS; ++i)
		{
			mSsboObbsData[i] = mSsboObbsData[0] + ((size * i) / sizeof(float));
		}

		for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
		{
			const MeshRendererComponent& bMesh = *mMeshComponents[i].component;
			float3 points[8];
			bMesh.GetOriginalAABB().GetCornerPoints(points);
			for (int k = 0; k < 8; ++k)
			{
				memcpy(&mSsboObbsData[0][i * 32 + k * 4], points[k].ptr(), sizeof(float3));
			}
			//bMesh.GetOriginalAABB().GetCornerPoints(reinterpret_cast<float3*>(&mSsboObbsData[0][i * 32]));
		}
		for (int i = 1; i < NUM_BUFFERS; ++i)
		{
			memcpy(mSsboObbsData[i], mSsboObbsData[0], size);
		}
	}

	glDeleteBuffers(1, &mSkinSsboObbs);
	glGenBuffers(1, &mSkinSsboObbs);
	if (mNumSkins)
	{
		unsigned int size = mNumSkins * sizeof(float) * 32;
		if (size)
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSkinSsboObbs);
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, flags);
			mSkinSsboObbsData[0] = reinterpret_cast<float*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, flags));
			for (int i = 1; i < NUM_BUFFERS; ++i)
			{
				mSkinSsboObbsData[i] = mSkinSsboObbsData[0] + ((size * i) / sizeof(float));
			}
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	mPersistentsFlag = false;
}

unsigned int GeometryBatch::GetCommandsSsbo() const
{
	unsigned int ret;
	glGenBuffers(1, &ret);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ret);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mMeshComponents.size() * sizeof(Command), nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParameterBuffer);
	unsigned int startValue = 0;
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(startValue), &startValue);

	return ret;
}

void GeometryBatch::ComputeCommands(unsigned int bufferIdx, const math::Frustum& frustum)
{
	const unsigned int idx = mDrawCount % NUM_BUFFERS;
	glUseProgram(App->GetOpenGL()->GetSelectCommandsProgramId());
	glUniform1ui(0, mMeshComponents.size());
	float planes[24];
	frustum.GetPlanes(reinterpret_cast<Plane*>(planes));
	glUniform4fv(1, 6, planes);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 22, bufferIdx);
	unsigned int sizeMatIdxs = mMeshComponents.size() * sizeof(unsigned int);
	sizeMatIdxs += ALIGNED_STRUCT_SIZE(sizeMatIdxs, mSsboAligment) - sizeMatIdxs;
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 13, mSsboIndicesCommands, sizeMatIdxs, mMeshComponents.size()* sizeof(Command));
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, mSsboModelMatrices, idx * mMeshComponents.size() * sizeof(float) * 16, mMeshComponents.size() * sizeof(float) * 16);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 14, mSsboObbs, idx * mMeshComponents.size() * sizeof(float) * 32, mMeshComponents.size() * sizeof(float) * 32);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 23, mParameterBuffer);
	glDispatchCompute((mMeshComponents.size() + 63) / 64, 1, 1);
	
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, mParameterBuffer);
	//unsigned int startValue = 1;
	//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(startValue), &startValue);
	mIboFlag = true;
}

void GeometryBatch::RecreateSkinningSsbos()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSkinSsbo);
	if (mSkinBufferSize)
	{
		glBufferData(GL_SHADER_STORAGE_BUFFER, mSkinBufferSize, nullptr, GL_STATIC_DRAW);
		char* memory = reinterpret_cast<char*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
		unsigned int offset = 0;
		for (int i = 0; i < mUniqueMeshes.size(); ++i)
		{
			BatchMeshResource& mesh = mUniqueMeshes[i];
			if (mesh.HasSkinning())
			{
				unsigned int size;
				mesh.resource->GetInterleavedData(Attribute::Usage::ANIMATION, reinterpret_cast<float*>(memory + offset), &size);
				mesh.skinOffset = offset / mesh.resource->GetVertexSize(Attribute::Usage::ANIMATION);
				offset += size;
			}
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	glDeleteBuffers(1, &mSkinDispatchIndirectBuffer);
	glGenBuffers(1, &mSkinDispatchIndirectBuffer);
	if (mNumSkins)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSkinDispatchIndirectBuffer);
		unsigned int size = ALIGNED_STRUCT_SIZE(mNumSkins * sizeof(unsigned int) * 3, mSsboAligment);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, size * NUM_BUFFERS, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		mSkinDispatchIndirectBufferData[0] = reinterpret_cast<unsigned int*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size * NUM_BUFFERS, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
		for (int i = 1; i < NUM_BUFFERS; ++i)
		{
			mSkinDispatchIndirectBufferData[i] = mSkinDispatchIndirectBufferData[0] + ((size * i) / sizeof(unsigned int));
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	mSkinningFlag = false;
}

void GeometryBatch::RecreateVboAndEbo()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);
	if(mVboDataSize)
		glBufferData(GL_ARRAY_BUFFER, mVboDataSize, nullptr, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, 1, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
	if (mEboDataSize)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mEboDataSize, nullptr, GL_STATIC_DRAW);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1, nullptr, GL_STATIC_DRAW);
	char* vboBuffer = reinterpret_cast<char*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	unsigned int vboOffset = 0;
	unsigned int eboOffset = 0;
	for (int i = 0; i < mUniqueMeshes.size(); ++i)
	{
		BatchMeshResource& res = mUniqueMeshes[i];
		res.baseVertex = vboOffset / mVertexSize;
		res.firstIndex = eboOffset / sizeof(unsigned int);
		unsigned int size;
		res.resource->GetInterleavedData(Attribute::Usage::RENDER, reinterpret_cast<float*>(vboBuffer + vboOffset), &size);
		vboOffset += size;
		size = res.resource->GetNumberIndices() * sizeof(unsigned int);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, eboOffset, size, res.resource->GetIndices());
		eboOffset += size;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mVBOFlag = false;
}

void GeometryBatch::RecreateMaterials()
{
	//TODO: fer un glMapBuffer !!!
	unsigned int materialSize = ALIGNED_STRUCT_SIZE(sizeof(Material), sizeof(float)*3);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSsboMaterials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mUniqueMaterials.size() * materialSize, nullptr, GL_STATIC_DRAW);
	unsigned int offset = 0;
	for (const BatchMaterialResource& rMaterial : mUniqueMaterials) {
		Material material;
		memcpy(material.baseColor, rMaterial.resource->GetBaseColorFactor().ptr(), sizeof(float) * 3);
		memcpy(material.emissiveFactor, rMaterial.resource->GetEmissiveFactor().ptr(), sizeof(float) * 3);
		material.baseColorTex = (rMaterial.resource->GetBaseColorTexture()) ? rMaterial.resource->GetBaseColorTexture()->GetTextureHandle() : 0;
		material.metalRoughTex = (rMaterial.resource->GetMetallicRoughnessTexture()) ? rMaterial.resource->GetMetallicRoughnessTexture()->GetTextureHandle() : 0;
		material.normalTex = (rMaterial.resource->GetNormalTexture()) ? rMaterial.resource->GetNormalTexture()->GetTextureHandle() : 0;
		material.emissiveTex = (rMaterial.resource->GetEmissiveTexture()) ? rMaterial.resource->GetEmissiveTexture()->GetTextureHandle() : 0;
		material.hasMetalRoughTex = rMaterial.resource->IsMetallicRoughnessEnabled();
		material.hasBaseColorTex = rMaterial.resource->IsBaseColorEnabled();
		material.hasNormalMap = rMaterial.resource->IsNormalMapEnabled();
		material.hasEmissiveTex = rMaterial.resource->IsEmissiveEnabled();
		material.metalness = rMaterial.resource->GetMetallicFactor();
		material.roughness = rMaterial.resource->GetRoughnessFactor();

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Material), &material);
		offset += materialSize;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	mMaterialFlag = false;
}


void GeometryBatch::AddUniqueMesh(const MeshRendererComponent& cMesh, unsigned int& meshIdx)
{
	meshIdx = mUniqueMeshes.size();
	const ResourceMesh& rMesh = *cMesh.GetResourceMesh();
	mUniqueMeshes.emplace_back(&rMesh, 0, 0);
	mVBOFlag = true;
	mVboDataSize += rMesh.GetNumberVertices() * mVertexSize;
	mEboDataSize += rMesh.GetNumberIndices() * sizeof(unsigned int);
}

void GeometryBatch::AddMeshComponent(const MeshRendererComponent& cMesh)
{
	bool foundMaterial = false;
	uint32_t matIdx = 0;
	const ResourceMaterial& resourceMaterial = *cMesh.GetResourceMaterial();
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
		if (resourceMaterial.GetBaseColorTexture())
		{
			resourceMaterial.GetBaseColorTexture()->GenerateMipmaps();
			resourceMaterial.GetBaseColorTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetEmissiveTexture())
		{
			resourceMaterial.GetEmissiveTexture()->GenerateMipmaps();
			resourceMaterial.GetEmissiveTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetMetallicRoughnessTexture())
		{
			resourceMaterial.GetMetallicRoughnessTexture()->GenerateMipmaps();
			resourceMaterial.GetMetallicRoughnessTexture()->MakeTextutureBindless();
		}
		if (resourceMaterial.GetNormalTexture())
		{
			resourceMaterial.GetNormalTexture()->GenerateMipmaps();
			resourceMaterial.GetNormalTexture()->MakeTextutureBindless();
		}
		mMaterialFlag = true;  
	}

	unsigned int meshIdx = 0;
	//const AnimationComponent* cAnim = cMesh->GetOwner()->FindAnimationComponent();
	
	const ResourceMesh& rMesh = *cMesh.GetResourceMesh();
	
	if (rMesh.HasAttribute(Attribute::JOINT) && rMesh.HasAttribute(Attribute::WEIGHT))
	{
		AddUniqueMesh(cMesh, meshIdx);
		mUniqueMeshes.back().skinOffset = 1;
		mSkinBufferSize += rMesh.GetVertexSize(Attribute::Usage::ANIMATION) * rMesh.GetNumberVertices();
		mSkinningFlag = true;
		++mNumSkins;
	}
	else
	{
		bool foundMesh = false;
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
			AddUniqueMesh(cMesh, meshIdx);
		}
	}
	mMeshComponents.emplace_back(meshIdx, matIdx, &cMesh);
	mPersistentsFlag = true;
}

bool GeometryBatch::RemoveMeshComponent(const MeshRendererComponent& component)
{
	unsigned int bMeshIdx = 0;
	unsigned int bMaterialIdx = 0;
	bool found = false;
	for (auto it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
	{
		if (component.GetID() == it->component->GetID())
		{
			found = true;
			bMeshIdx = it->bMeshIdx;
			bMaterialIdx = it->bMaterialIdx;
			if (component.HasSkinning())
				mSkinningFlag = true;
			mMeshComponents.erase(it);
			break;
		}
	}
	if (!found)
		return false;

	const ResourceMesh& rMesh = *component.GetResourceMesh();
	if (!--mUniqueMeshes[bMeshIdx].referenceCount)
	{
		if (mUniqueMeshes[bMeshIdx].HasSkinning())
		{
			mSkinBufferSize -= rMesh.GetNumberVertices() * rMesh.GetVertexSize(Attribute::Usage::ANIMATION);
			mSkinningFlag = true;
			--mNumSkins;
		}
		mUniqueMeshes.erase(mUniqueMeshes.begin() + bMeshIdx);
		for (auto it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
		{
			if (it->bMeshIdx > bMeshIdx)
			{
				--(it->bMeshIdx);
			}
		}
		mVboDataSize -= rMesh.GetNumberVertices() * mVertexSize;
		mEboDataSize -= rMesh.GetNumberIndices() * sizeof(unsigned int);
		mVBOFlag = true;
	}
	if (!--mUniqueMaterials[bMaterialIdx].referenceCount)
	{
		mUniqueMaterials.erase(mUniqueMaterials.begin() + bMaterialIdx);
		mMaterialFlag = true;
		for (auto it = mMeshComponents.begin(); it != mMeshComponents.end(); ++it)
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

void GeometryBatch::Draw(unsigned int programId, const math::Frustum& frustum)
{
	if (mMeshComponents.size() == 0)
		return;

	unsigned int ibo = GetCommandsSsbo();
	ComputeCommands(ibo, frustum);

	if (mSkinningApplied)
	{
		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
		mSkinningApplied = false;
	}
	if (mIboFlag)
	{
		glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
		mIboFlag = false;
	}

	//Wait for GPU
	unsigned int idx = mDrawCount % NUM_BUFFERS;
	if (mSync[idx])
	{
		GLenum waitReturn = GL_UNSIGNALED;
		while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED)
		{
			waitReturn = glClientWaitSync(mSync[idx], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
		}
	}

	glUseProgram(programId);
	glBindVertexArray(mVao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibo);
	glBindBuffer(GL_PARAMETER_BUFFER, mParameterBuffer);

	unsigned int structSize = ALIGNED_STRUCT_SIZE(sizeof(float) * 16, mSsboAligment);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 10, mSsboModelMatrices, idx * mMeshComponents.size() * structSize, mMeshComponents.size() * structSize);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, mSsboMaterials);
	structSize = ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), 4);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 12, mSsboIndicesCommands, 0, ALIGNED_STRUCT_SIZE(sizeof(BufferIndices), 4) * mMeshComponents.size());
	glMultiDrawElementsIndirectCount(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, 0, mMeshComponents.size(), 0);

	//CleanUp
	glBindVertexArray(0);
	glDeleteBuffers(1, &ibo);
}

void GeometryBatch::Update(const std::vector<const math::Frustum*>& frustums)
{
	if (mVBOFlag)
	{
		RecreateVboAndEbo();
	}
	if (mMaterialFlag)
	{
		RecreateMaterials();
	}
	if (mPersistentsFlag)
	{
		RecreatePersistentSsbos();
	}
	if (mSkinningFlag)
	{
		RecreateSkinningSsbos();
	}
	if (frustums.size() > mFrustumsSsboCapacity)
	{
		RecreatePersistentFrustums();
	}

	const unsigned int idx = mDrawCount % NUM_BUFFERS;
	for (int i = 0; i < frustums.size(); ++i)
	{
		frustums[i]->GetPlanes(reinterpret_cast<Plane*>(mSsboFrustumsData[idx] + 24 * i));
	}
	unsigned int currSkin = 0;
	for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
	{
		const BatchMeshRendererComponent& bComp = mMeshComponents[i];
		//ComputeSkinning(bComp);

		const BatchMeshResource& bRes = mUniqueMeshes[bComp.bMeshIdx];
		if (bComp.component->HasSkinning())
		{
			memcpy(mSsboModelMatricesData[idx] + 16 * bComp.baseInstance, float4x4::identity.ptr(), sizeof(float) * 16);
			//Transform the obb points as we set identity as the model matrix
			math::OBB obb = bComp.component->GetOriginalAABB().Transform(bComp.component->GetOwner()->GetWorldTransform());
			float3 points[8];
			obb.GetCornerPoints(points);
			for (int k = 0; k < 8; ++k)
			{
				memcpy(&mSsboObbsData[idx][i * 32 + k * 4], points[k].ptr(), sizeof(float3));
			}
			//obb.GetCornerPoints(reinterpret_cast<float3*>(&mSsboObbsData[idx][i * 32]));
			memcpy(&mSkinSsboObbsData[idx][currSkin * 32], &mSsboObbsData[idx][i * 32], sizeof(float3) * 8);
			mSkinDispatchIndirectBufferData[idx][currSkin*3] = (bComp.component->GetResourceMesh()->GetNumberVertices() + 63)/64;
			mSkinDispatchIndirectBufferData[idx][currSkin*3 + 1] = 1;
			mSkinDispatchIndirectBufferData[idx][currSkin*3 + 2] = 1;
			++currSkin;
		}
		else
		{
			memcpy(mSsboModelMatricesData[idx] + 16 * bComp.baseInstance, bComp.component->GetOwner()->GetWorldTransform().ptr(), sizeof(float) * 16);
		}
	}
	if (mNumSkins)
	{
		glUseProgram(App->GetOpenGL()->GetSelectSkinsProgramId());
		glUniform1ui(0, mNumSkins);
		glUniform1ui(1, frustums.size());
		const unsigned int sizeFrustums = frustums.size() * 24 * sizeof(float);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 18, mFrustumsSsbo, idx * sizeFrustums, sizeFrustums);
		unsigned int sizeIndirectBuffers = ALIGNED_STRUCT_SIZE(mNumSkins * sizeof(unsigned int) * 3, mSsboAligment);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 16, mSkinDispatchIndirectBuffer, sizeIndirectBuffers * idx, sizeIndirectBuffers);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 17, mSkinSsboObbs, idx * mNumSkins * 32 * sizeof(float), mNumSkins * 32 * sizeof(float));
		glDispatchCompute((mNumSkins + 63) / 64, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, mSkinDispatchIndirectBuffer);
		for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
		{
			if (mMeshComponents[i].component->HasSkinning())
			{
				ComputeSkinning(mMeshComponents[i]);
			}
		}
	}
}

void GeometryBatch::EndFrameDraw()
{
	unsigned int idx = mDrawCount % NUM_BUFFERS;
	glDeleteSync(mSync[idx]);
	mSync[idx] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	++mDrawCount;
	mCurrSkinIdx = 0;
}

void GeometryBatch::ComputeSkinning(const BatchMeshRendererComponent& bMesh)
{
	const MeshRendererComponent& cMesh = *bMesh.component;
	const ResourceMesh* rMesh = cMesh.GetResourceMesh();
	glUseProgram(App->GetOpenGL()->GetSkinningProgramId());
	const BatchMeshResource& bRes = mUniqueMeshes[bMesh.bMeshIdx];
	//TODO: El buffer range de los vertices dskin entre 2 batches no funcionara
	assert(bRes.skinOffset != -1 && "Skin mesh does not have the offset set");
	glUniform1ui(0, bRes.skinOffset);

	const std::vector<float4x4>& palette = cMesh.GetPalette();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mPaletteSsbo);
	if (palette.size() > mBiggestPaletteSize)
	{
		glBufferData(GL_SHADER_STORAGE_BUFFER, palette.size() * sizeof(float) * 16, palette.data(), GL_DYNAMIC_DRAW);
		mBiggestPaletteSize = palette.size();
	}
	else
	{
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, palette.size() * sizeof(float) * 16, palette.data());
	}
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 21, mVbo, bRes.baseVertex * mVertexSize, mVertexSize * rMesh->GetNumberVertices());
	glUniform1i(25, rMesh->GetNumberVertices());
	glDispatchComputeIndirect(((mDrawCount % NUM_BUFFERS) * ALIGNED_STRUCT_SIZE(mNumSkins * sizeof(unsigned int) * 3, mSsboAligment)) + sizeof(unsigned int)*3*mCurrSkinIdx++);
	//glDispatchCompute((rMesh->GetNumberVertices() + 63) / 64, 1, 1);
	mSkinningApplied = true;
}