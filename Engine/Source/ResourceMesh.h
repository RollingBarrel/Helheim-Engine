#pragma once

#include <vector>

#include "ImporterMesh.h"
#include "Resource.h"

typedef struct Attribute {
	enum Type : unsigned char {
		POS = 1 << 0,
		UV = 1 << 1,
		NORMAL = 1 << 2,
		TANGENT = 1 << 3,
		COLOR = 1 << 4,
	};
	Attribute(Type iType, unsigned int iSize, unsigned int iOffset) : size(iSize), offset(iOffset), type(iType) {}
	Attribute(const Attribute& other) : size(other.size), offset(other.offset), type(other.type) {}
	unsigned int size;
	unsigned int offset;
	Type type;
}Attribute;

float* GetAttributeDataFromInterleavedBuffer(Attribute attr, float* interleavedBuffer, unsigned int bufferSize, unsigned int vertexSize);


class ResourceMesh : public Resource
{
public:

	ResourceMesh(unsigned int uid,
		unsigned int numIndices,
		unsigned int numVertices);

	//ResourceMesh(const ResourceMesh& other);
	~ResourceMesh();

	unsigned int GetNumberVertices() const { return mNumVertices; }
	unsigned int GetNumberIndices() const { return mNumIndices; }
	const unsigned int* GetIndices() const { return mIndices; }
	const std::vector<Attribute>& GetAttributes() const { return mAttributes; }
	unsigned int GetNumAttributtes() const { return mAttributes.size(); }
	unsigned int GetVertexSize() const { return mVertexSize; }
	const float* GetAttributeData(Attribute::Type type) const;
	int GetAttributeIdx(Attribute::Type type) const;
	//This allocates memory in the return pointer that you must free
	float* GetInterleavedData() const;

	bool LoadInterleavedAttribute(float* fillBuffer, const Attribute& attribute, unsigned int vertexSize) const;
	void AddAttribute(const Attribute& attribute, float* attributeData, unsigned int dataSize);
	void AddAttribute(const Attribute& attribute, float*&& attributeData);
	void GenerateTangents();

private:
	void CleanUp();

	unsigned int* mIndices = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	std::vector<float*> mAttributesData;
	std::vector<Attribute> mAttributes;
	unsigned int mVertexSize = 0;
};

