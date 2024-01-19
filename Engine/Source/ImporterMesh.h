#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
#include <vector>


struct ResourceMesh;

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* ourMesh);

		void Save(const ResourceMesh* ourMesh);

		void Load(char* fileBuffer, ResourceMesh* ourMesh, const char* fileName);
	}
}

typedef struct Attribute {
	enum Type : unsigned char {
		POS = 1 << 0,
		UV = 1 << 1,
		NORMAL = 1 << 2,
		TANGENT = 1 << 3,
		COLOR = 1 << 4,
	};
	Attribute(Type iType, unsigned int iSize, unsigned int iOffset) : type(iType), size(iSize), offset(iOffset) {}
	Attribute(const Attribute& other): type(other.type), size(other.size), offset(other.offset) {}
	Type type;
	unsigned int size;
	unsigned int offset;
}Attribute;

struct ResourceMesh
{
	ResourceMesh(const ResourceMesh& other);
	~ResourceMesh() { CleanUp(); }
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	unsigned int* mIndices = nullptr;
	std::vector<float*> mAttributesData;


	const char* mMeshName = nullptr;
	unsigned int mUID = 0;

	void FromInterleavedData(float* vData, unsigned int numVertices, unsigned int* iData, unsigned int numIndices, Attribute* attributes = nullptr);
	float* GetInterleavedData() const;
	unsigned int GetVertexSize() const { return mVertexSize; }

	int AttributeIdx(Attribute::Type type) const;
	void AddAttribute(const Attribute& attribute, float* attributeData);
	const float* GetAttributData(Attribute::Type type) const;

	void LoadVAO();
	void LoadVBO();
	void LoadEBO();

	void CleanUp();

private:
	void LoadInterleavedAttribute(float* fillBuffer, const float* attribData, unsigned int& attribFloatsOffset, unsigned int attribElements, unsigned int elementFloats) const;

	unsigned int mVao;
	unsigned int mVbo;
	unsigned int mEbo;
	std::vector<Attribute*> mAttributes;
	unsigned int mVertexSize = 0;

	friend void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh);
	friend void Importer::Mesh::Load(char* fileBuffer, ResourceMesh* mesh, const char* fileName);
};
