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
		//WARNING!!: (JOINT ATTRIBUTE IS NOT A FLOAT!!!) When requesting it you have to cast it to unsigned int !!!
		JOINT = 1 << 4,
		WEIGHT = 1 << 5,
		COLOR = 1 << 6 
	};
	enum Usage : unsigned char {
		ANIMATION = 1 << 1,
		RENDER = 1 << 2,
		ALL = 255,
	};
	Attribute(Type iType, unsigned int iSize, unsigned int iOffset, Usage iUsage = Usage::ALL) : size(iSize), offset(iOffset), type(iType), usage(iUsage) {}
	Attribute(const Attribute& other) : size(other.size), offset(other.offset), type(other.type), usage(other.usage) {}
	unsigned int size;
	unsigned int offset;
	Type type;
	Usage usage;
}Attribute;

class ENGINE_API ResourceMesh : public Resource
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
	unsigned int GetNumberAttributes() const { return static_cast<unsigned int>(mAttributes.size()); }
	bool HasAttribute(Attribute::Type type) const;
	unsigned int GetVertexSize() const { return mVertexSize; }
	const float* GetAttributeData(Attribute::Type type) const;
	int GetAttributeIdx(Attribute::Type type) const;
	//This allocates memory in the return pointer that you must delete
	float* GetInterleavedData(Attribute::Usage usage) const;

private:
	unsigned int* mIndices = nullptr;
	unsigned int mNumVertices = 0;
	unsigned int mNumIndices = 0;

	std::vector<float*> mAttributesData;
	std::vector<Attribute> mAttributes;
	unsigned int mVertexSize = 0;
};

#endif // _RESOURCE_MESH_H_