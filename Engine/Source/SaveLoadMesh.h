class ResourceMesh;

namespace Importer
{
	namespace Mesh
	{
		void Save(const ResourceMesh* ourMesh);

		ResourceMesh* Load(const char* fileName, unsigned int uid);
	}
}