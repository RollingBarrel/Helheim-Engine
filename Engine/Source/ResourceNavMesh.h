#pragma once

//TODO: does not inherit from resource ??
struct ResourceNavMesh
{
	unsigned int mUID = 0;
	unsigned short* meshVertices = nullptr;	///< The mesh vertices. [Form: (x, y, z) * #numberOfVertices]
	unsigned short* polygons = nullptr;	///< Polygon and neighbor data. [Length: #maxPolygons * 2 * #verticesPerPolygon]
	unsigned short* polygonRegionID = nullptr;	///< The region id assigned to each polygon. [Length: #maxPolygons]
	unsigned short* polygonFlags = nullptr;	///< The user defined flags for each polygon. [Length: #maxPolygons]
	unsigned char* polygonAreaID = nullptr;	///< The area id assigned to each polygon. [Length: #maxPolygons]
	int numberOfVertices = 0;				///< The number of vertices.
	//int numberOfPolygons = 0;				///< The number of polygons. (Maybe not necesary having the maypolygons?)
	int maxPolygons = 0;			///< The number of allocated polygons.
	int verticesPerPolygon = 0;				///< The maximum number of vertices per polygon.
	float boundMin[3] = { 0.0f,0.0f,0.0f };			///< The minimum bounds in world space. [(x, y, z)]
	float boundMax[3] = { 0.0f,0.0f,0.0f };			///< The maximum bounds in world space. [(x, y, z)]
	float cellSize = 0.0f;				///< The size of each cell. (On the xz-plane.)
	float cellHeight = 0.0f;				///< The height of each cell. (The minimum increment along the y-axis.)
	int borderSize = 0;			///< The AABB border size used to generate the source data from which the mesh was derived.
	float maxEdgeError = 0.0f;		///< The max error of the polygon edges in the mesh.
};