#include "Application.h"
#include "ImporterNavMesh.h"
#include "ResourceNavMesh.h"
#include "SaveLoadNavMesh.h"
#include "DetourNavMesh.h"
#include "ModuleFileSystem.h"


ResourceNavMesh* Importer::NavMesh::Import(unsigned int uid,const char* assetsFile)
{
	const char* libPath = App->GetFileSystem()->GetLibraryFile(uid, true);
	App->GetFileSystem()->CopyAbsolutePath(assetsFile, libPath);
	ResourceNavMesh* rNavMesh = Load(libPath, uid);
	delete[] libPath;
	return rNavMesh;
}

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

void Importer::NavMesh::SaveAsset(const char* path, const dtNavMesh& mesh)
{
	unsigned int size = 0;
	unsigned int byteSize = 0;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh.getParams(), sizeof(dtNavMeshParams));
	size += sizeof(NavMeshSetHeader);

	// Store tiles.
	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh.getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;

		size += sizeof(tile->dataSize);
		size += tile->dataSize;
	}


	char* fileBuffer = new char[size];
	char* cursor = fileBuffer;

	unsigned int bytes = sizeof(NavMeshSetHeader);
	memcpy(cursor, &header, bytes);
	cursor += bytes;
	byteSize += bytes;
	int maxTiles = mesh.getMaxTiles();
	for (int i = 0; i < maxTiles; ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh.getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		bytes = sizeof(tile->dataSize);
		memcpy(cursor, &tileHeader.dataSize, bytes);
		cursor += bytes;
		byteSize += bytes;
		bytes = tile->dataSize;
		memcpy(cursor, tile->data, bytes);
		if (i + 1 < maxTiles)
		{
			cursor += bytes;
			byteSize += bytes;
		}

	}
	App->GetFileSystem()->Save(path, fileBuffer, size);
	delete[] fileBuffer;
}