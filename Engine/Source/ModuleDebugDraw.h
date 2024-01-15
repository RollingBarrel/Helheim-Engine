#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"
#include "MathGeoLibFwd.h"

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

    void SetDrawGrid(bool drawGrid) { mDrawGrid = drawGrid; }
    bool GetDrawGrid() const { return mDrawGrid; }
    void Draw(const float4x4& viewproj, unsigned width, unsigned height);
    void DrawBoundingBox(const OBB& obb);
    void DrawQuadtree(const AABB& aabb);


private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mDrawGrid; 


    void DrawGrid();
    void DrawFrustum(const Frustum& frustum); 

};

#endif /* _MODULE_DEBUGDRAW_H_ */
