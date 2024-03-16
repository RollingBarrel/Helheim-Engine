#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"
#include "MathGeoLibFwd.h"


class DDRenderInterfaceCoreGL;
class Camera;
class GameObject;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw();

	bool            Init();
	update_status   Update(float dt);
	bool            CleanUp();

    void Draw(const float4x4& viewproj, unsigned width, unsigned height);
    void DrawAxis();
    void DrawFrustum(const Frustum& frustum);
    bool GetShouldRenderGrid() const { return mDrawGrid; }
    void SetRenderGrid(bool a) { mDrawGrid = a; }
    void DrawCube(const OBB& obb, const float3& color);
    void DrawSphere(const float center[3], const float color[3], const float radius);
    void DrawCone(const float pos[3], const float dir[3], const float color[3], const float bRadius);
    void DrawLine(const float3& position, const float3& direction, const float3& color);
    void DrawTriangle(const float3& v1, const float3& v2, const float3& v3);

    void DrawSkeleton(GameObject* model);


private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mDrawGrid = true;

    void DrawGrid();
    


};
#endif /* _MODULE_DEBUGDRAW_H_ */
