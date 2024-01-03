#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Geometry/OBB.h"

class DDRenderInterfaceCoreGL;
class Camera;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw();

	bool            Init();
	update_status   Update();
	bool            CleanUp();

    void SetDrawGrid(bool drawGrid);
    bool GetDrawGrid() const;
    void Draw(const float4x4& viewproj, unsigned width, unsigned height);


private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mdrawGrid; 


    void DrawBoundingBox(const OBB& obb);
    void DrawGrid();
    void DrawFrustum(const Frustum& frustum); 

};

inline void ModuleDebugDraw::SetDrawGrid(bool drawGrid)
{
    mdrawGrid = drawGrid;
}

inline bool ModuleDebugDraw::GetDrawGrid() const
{
    return mdrawGrid ;
}
#endif /* _MODULE_DEBUGDRAW_H_ */
