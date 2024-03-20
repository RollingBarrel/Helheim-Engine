#include "ImporterNavMesh.h"
#include <vector>
#include "Application.h"
#include "ModuleFileSystem.h"
void Importer::NavMesh::Import()
{
	//NOT NEEDED 
}

void Importer::NavMesh::Save(const ResourceNavMesh& navMesh)
{

	int header[3]={ navMesh.numberOfVertices, navMesh.maxPolygons, navMesh.verticesPerPolygon };
	float cellData[2] = { navMesh.cellSize,navMesh.cellHeight };
	unsigned int totalVerticesSize = sizeof(unsigned short) * navMesh.numberOfVertices;
	unsigned int totalPoligonsSize = sizeof(unsigned short) * navMesh.maxPolygons * 2 * navMesh.verticesPerPolygon;
	unsigned int polygonRegionIdSize = sizeof(unsigned short) * navMesh.maxPolygons;
	unsigned int polygonFlagsSize = sizeof(unsigned short) * navMesh.maxPolygons;
	unsigned int polygonAreaIdSize = sizeof(unsigned short) * navMesh.maxPolygons;

	unsigned int size = sizeof(header) + sizeof(cellData) + totalVerticesSize + 
		totalPoligonsSize + polygonRegionIdSize + polygonFlagsSize + polygonAreaIdSize + 
		sizeof(navMesh.boundMin)+ sizeof(navMesh.boundMax) + sizeof(navMesh.borderSize) + sizeof(navMesh.maxEdgeError);

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
	memcpy(cursor,&navMesh.meshVertices, bytes);
	cursor += bytes;

	//Save Polygons
	bytes = totalPoligonsSize;
	memcpy(cursor, &navMesh.polygons, bytes);
	cursor += bytes;

	//Save PolygonRegionId
	bytes = polygonRegionIdSize;
	memcpy(cursor, &navMesh.polygonRegionID, bytes);
	cursor += bytes;

	//Save PolygonFlags
	bytes = polygonFlagsSize;
	memcpy(cursor, &navMesh.polygonFlags, bytes);
	cursor += bytes;

	//Save PolygonAreaID
	bytes = polygonAreaIdSize;
	memcpy(cursor, &navMesh.polygonAreaID, bytes);
	cursor += bytes;

	//Save boundMin
	bytes = sizeof(navMesh.boundMin);
	memcpy(cursor, navMesh.boundMin, bytes);
	cursor += bytes;

	//Save boundMax
	bytes = sizeof(navMesh.boundMax);
	memcpy(cursor, navMesh.boundMax, bytes);
	cursor += bytes;

	//Save BorderSize
	bytes = sizeof(navMesh.borderSize);
	memcpy(cursor, &navMesh.borderSize, bytes);
	cursor += bytes;

	//Save MaxEdgeError
	bytes = sizeof(navMesh.maxEdgeError);
	memcpy(cursor, &navMesh.maxEdgeError, bytes);
	cursor += bytes;


	std::string path = LIBRARY_NAVMESH_PATH;
	path += std::to_string(navMesh.mUID);
	path += ".navmesshi";

	App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

	delete[] fileBuffer;
	fileBuffer = nullptr;
}

void Importer::NavMesh::Load(ResourceNavMesh& navMesh, const char* fileName)
{
	std::string path = LIBRARY_NAVMESH_PATH;
	path += fileName;
	path += ".navmesshi";
	char* fileBuffer = nullptr;
	//App->GetFileSystem()->Load(path.c_str(), &fileBuffer);
	if (App->GetFileSystem()->Load(path.c_str(), &fileBuffer))
	{
		//Load Header
		char* cursor = fileBuffer;
		unsigned int header[3];
		unsigned int bytes = sizeof(header);
		memcpy(header, cursor, bytes);
		cursor += bytes;
		navMesh.numberOfVertices = header[0];
		navMesh.maxPolygons = header[1];
		navMesh.verticesPerPolygon = header[2];

		//Load Celldata
		float cellData[2];
		bytes = sizeof(cellData);
		memcpy(cellData, cursor, bytes);
		cursor += bytes;
		navMesh.cellSize = cellData[0];
		navMesh.cellHeight = cellData[1];

		//Load Vertices
		bytes = sizeof(unsigned short) * navMesh.numberOfVertices;
		memcpy(&navMesh.meshVertices, cursor, bytes);
		cursor += bytes;

		//Load Polygons
		bytes = sizeof(unsigned short) * navMesh.maxPolygons * 2 * navMesh.verticesPerPolygon;
		memcpy(&navMesh.polygons, cursor, bytes);
		cursor += bytes;

		//Load PolygonRegionId
		bytes = sizeof(unsigned short) * navMesh.maxPolygons;
		memcpy(&navMesh.polygonRegionID, cursor, bytes);
		cursor += bytes;

		//Load PolygonFlags
		bytes = sizeof(unsigned short) * navMesh.maxPolygons;
		memcpy(&navMesh.polygonFlags, cursor, bytes);
		cursor += bytes;

		//Load PolygonAreaID
		bytes = sizeof(unsigned short) * navMesh.maxPolygons;
		memcpy(&navMesh.polygonAreaID, cursor, bytes);
		cursor += bytes;

		//Load boundMin
		bytes = sizeof(navMesh.boundMin);
		memcpy(navMesh.boundMin, cursor, bytes);
		cursor += bytes;

		//Load boundMax
		bytes = sizeof(navMesh.boundMax);
		memcpy(navMesh.boundMax, cursor, bytes);
		cursor += bytes;

		//Load BorderSize
		bytes = sizeof(int);
		memcpy(&navMesh.borderSize, cursor, bytes);
		cursor += bytes;

		//Load MaxEdgeError
		bytes = sizeof(float);
		memcpy(&navMesh.maxEdgeError, cursor, bytes);
		cursor += bytes;

		delete[] fileBuffer;
	}
}
