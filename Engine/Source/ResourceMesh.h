#ifndef _RESOURCE_MESH_H_
#define _RESOURCE_MESH_H_

#include <vector>
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

class ResourceMesh : public Resource
{
public:

	ResourceMesh(unsigned int uid, unsigned int numIndices, const unsigned int* indices, unsigned int numVertices, const std::vector<Attribute>& attributes, const std::vector<float*>& attributesData);
	ResourceMesh(unsigned int uid, unsigned int numIndices, unsigned int*&& indices, unsigned int numVertices, std::vector<Attribute>&& attributes, std::vector<float*>&& attributesData);
	ResourceMesh(const ResourceMesh& other);
	ResourceMesh(ResourceMesh&& other);
	~ResourceMesh();

	unsigned int GetNumberVertices() const { return mNumVertices; }
	unsigned int GetNumberIndices() const { return mNumIndices; }
	const unsigned int* GetIndices() const { return mIndices; }
	void GetAttributes(std::vector<Attribute>&) const;
	unsigned int GetNumberAttributes() const { return mAttributes.size(); }
	bool HasAttribute(Attribute::Type type) const;
	unsigned int GetVertexSize() const { return mVertexSize; }
	const float* GetAttributeData(Attribute::Type type) const;
	int GetAttributeIdx(Attribute::Type type) const;
	//This allocates memory in the return pointer that you must delete
	float* GetInterleavedData() const;
	bool LoadInterleavedAttribute(float* interleavedBuffer, const Attribute& attribute, unsigned int vertexSize) const;

private:
	unsigned int* mIndices = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	std::vector<float*> mAttributesData;
	std::vector<Attribute> mAttributes;
	unsigned int mVertexSize = 0;
};

#endif // _RESOURCE_MESH_H_