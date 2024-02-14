#pragma once
struct ResourceNavMesh
{
	unsigned short* meshVertices;	///< The mesh vertices. [Form: (x, y, z) * #numberOfVertices]
	unsigned short* polygons;	///< Polygon and neighbor data. [Length: #maxPolygons * 2 * #verticesPerPolygon]
	unsigned short* polygonRegionID;	///< The region id assigned to each polygon. [Length: #maxPolygons]
	unsigned short* polygonFlags;	///< The user defined flags for each polygon. [Length: #maxPolygons]
	unsigned char* polygonAreaID;	///< The area id assigned to each polygon. [Length: #maxPolygons]
	int numberOfVertices;				///< The number of vertices.
	int numberOfPolygons;				///< The number of polygons. (Maybe not necesary having the maypolygons?)
	int maxPolygons;			///< The number of allocated polygons.
	int verticesPerPolygon;				///< The maximum number of vertices per polygon.
	float boundMin[3];			///< The minimum bounds in world space. [(x, y, z)]
	float boundMax[3];			///< The maximum bounds in world space. [(x, y, z)]
	float cellSize;				///< The size of each cell. (On the xz-plane.)
	float cellHeight;				///< The height of each cell. (The minimum increment along the y-axis.)
	int borderSize;			///< The AABB border size used to generate the source data from which the mesh was derived.
	float maxEdgeError;		///< The max error of the polygon edges in the mesh.
};

namespace Importer
{
    namespace NavMesh
    {
        void Import();
        void Save(const ResourceNavMesh* navMesh);
        void Load(ResourceNavMesh* ourNavMesh , const char* fileName = nullptr);
    }
};