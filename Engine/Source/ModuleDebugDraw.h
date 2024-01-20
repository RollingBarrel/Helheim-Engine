#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"
#include "MathGeoLibFwd.h"

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

    void Draw(const float4x4& viewproj, unsigned width, unsigned height);
    void DrawBoundingBox(const OBB& obb);
    void DrawQuadtree(const AABB& aabb);
    void DrawAxis();
    void DrawFrustum(const Frustum& frustum);
    bool* GetShouldRenderGrid() const { return mDrawGrid; }
    bool* GetShouldRenderAxis() const { return mDrawAxis; }

private:

    static DDRenderInterfaceCoreGL* implementation;
    bool* mDrawGrid = new bool(true);
    bool* mDrawAxis = new bool(true);

    void DrawGrid();
    void DrawFrustum(const Frustum& frustum); 


};
#endif /* _MODULE_DEBUGDRAW_H_ */
