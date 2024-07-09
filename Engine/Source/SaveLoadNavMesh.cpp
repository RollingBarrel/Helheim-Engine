#include "SaveLoadNavMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceNavMesh.h"
#include "DetourNavMesh.h"

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

void Importer::NavMesh::Save(ResourceNavMesh* navMesh)
{
	//Preguntarle a carlos por mas del const 
	const dtNavMesh* mesh = navMesh->GetDtNavMesh();
	if (!mesh) return;
	unsigned int size = 0;
	unsigned int byteSize = 0;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	size += sizeof(NavMeshSetHeader);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
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
	int maxTiles = mesh->getMaxTiles();
	for (int i = 0; i < maxTiles; ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
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
	const char* libPath = App->GetFileSystem()->GetLibraryFile(navMesh->GetUID(), true);
	App->GetFileSystem()->Save(libPath, fileBuffer, size);
	delete[] fileBuffer;
	delete[] libPath;

}

ResourceNavMesh* Importer::NavMesh::Load(const char* fileName, unsigned int uid)
{
	char* fileBuffer = nullptr;
	ResourceNavMesh* ret = nullptr;
	int bytes = 0;
	unsigned int byteSize = 0;
	if (App->GetFileSystem()->Load(fileName, &fileBuffer))
	{
		char* cursor = fileBuffer;
		NavMeshSetHeader header;
		bytes = sizeof(NavMeshSetHeader);
		memcpy(&header, cursor, bytes);
		cursor += bytes;
		byteSize += bytes;
		if (header.magic != NAVMESHSET_MAGIC)
		{
			return 0;
		}
		if (header.version != NAVMESHSET_VERSION)
		{
			return 0;
		}

		dtNavMesh* mesh = dtAllocNavMesh();

		if (!mesh)
		{
			return 0;
		}
		dtStatus status = mesh->init(&header.params);
		if (dtStatusFailed(status))
		{
			return 0;
		}
		for (int i = 0; i < header.numTiles; ++i)
		{
			NavMeshTileHeader tileHeader;
			bytes = sizeof(tileHeader.dataSize);
			memcpy(&tileHeader.dataSize, cursor, bytes);
			cursor += bytes;
			byteSize += bytes;
			bytes = tileHeader.dataSize;
			if (!tileHeader.dataSize)
				break;

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memset(data, 0, tileHeader.dataSize);
			bytes = tileHeader.dataSize;
			memcpy(data, cursor, bytes);
			cursor += bytes;
			byteSize += bytes;

			mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
		}
		ret = new ResourceNavMesh(uid, mesh);
		delete[] fileBuffer;
		
	}
	return ret;
}