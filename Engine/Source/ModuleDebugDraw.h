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

    void Draw(const float4x4& viewproj, unsigned width, unsigned height);
    void DrawBoundingBox(const OBB& obb);
    void DrawQuadtree(const AABB& aabb);
    void DrawAxis();
    bool GetShouldRenderGrid() const { return mDrawGrid; }
    void SetRenderGrid(bool a) { mDrawGrid = a; }
    void DrawSphere(const float center[3], const float color[3], const float radius);
    void DrawCone(const float pos[3], const float dir[3], const float color[3], const float bRadius);



private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mDrawGrid = true;

    void DrawGrid();
    void DrawFrustum(const Frustum& frustum); 


};
#endif /* _MODULE_DEBUGDRAW_H_ */
