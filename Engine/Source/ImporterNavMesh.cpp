#include "ImporterNavMesh.h"
#include <vector>
#include "Application.h"
#include "ModuleFileSystem.h"
void Importer::NavMesh::Import()
{
	//NOT NEEDED 
}

void Importer::NavMesh::Save(const ResourceNavMesh* navMesh)
{
	int header[]={ navMesh->numberOfVertices, navMesh->maxPolygons, navMesh->verticesPerPolygon };
	float cellData[] = { navMesh->cellSize,navMesh->cellHeight };
	unsigned int totalVerticesSize = sizeof(unsigned short) * navMesh->numberOfVertices;
	unsigned int totalPoligonsSize = sizeof(unsigned short) * navMesh->maxPolygons * 2 * navMesh->verticesPerPolygon;
	unsigned int polygonRegionIdSize = sizeof(unsigned short) * navMesh->maxPolygons;
	unsigned int polygonFlagsSize = sizeof(unsigned short) * navMesh->maxPolygons;
	unsigned int polygonAreaIdSize = sizeof(unsigned short) * navMesh->maxPolygons;

	unsigned int size = sizeof(header) + sizeof(cellData) + totalVerticesSize + 
		totalPoligonsSize + polygonRegionIdSize + polygonFlagsSize + polygonAreaIdSize + 
		sizeof(navMesh->boundMin)+ sizeof(navMesh->boundMax) + sizeof(navMesh->borderSize) + sizeof(navMesh->maxEdgeError);

	char* fileBuffer = new char[size];
	char* cursor = fileBuffer;

	//Save Header
	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;

	//Save Celldata
	bytes= sizeof(cellData);
	memcpy(cursor, cellData, bytes);
	cursor += bytes;

	//Save Vertices
	bytes = totalVerticesSize;
	memcpy(cursor,&navMesh->numberOfVertices, bytes);
	cursor += bytes;

	//Save Polygons
	bytes = totalPoligonsSize;
	memcpy(cursor, &navMesh->numberOfPolygons, bytes);
	cursor += bytes;

	//Save PolygonRegionId
	bytes = polygonRegionIdSize;
	memcpy(cursor, &navMesh->polygonRegionID, bytes);
	cursor += bytes;

	//Save PolygonFlags
	bytes = polygonFlagsSize;
	memcpy(cursor, &navMesh->polygonFlags, bytes);
	cursor += bytes;

	//Save PolygonAreaID
	bytes = polygonAreaIdSize;
	memcpy(cursor, &navMesh->polygonAreaID, bytes);
	cursor += bytes;

	//Save boundMin
	bytes = sizeof(navMesh->boundMin);
	memcpy(cursor, navMesh->boundMin, bytes);
	cursor += bytes;

	//Save boundMax
	bytes = sizeof(navMesh->boundMax);
	memcpy(cursor, navMesh->boundMax, bytes);
	cursor += bytes;

	//Save BorderSize
	bytes = sizeof(navMesh->borderSize);
	memcpy(cursor, &navMesh->borderSize, bytes);
	cursor += bytes;

	//Save MaxEdgeError
	bytes = sizeof(navMesh->maxEdgeError);
	memcpy(cursor, &navMesh->maxEdgeError, bytes);
	cursor += bytes;


	std::string path = LIBRARY_NAVMESH_PATH;
	//path += std::to_string(navMesh->mUID);
	path += ".navmesshi";

	App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

	delete[] fileBuffer;
	fileBuffer = nullptr;
}
