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

	bool            Init() override;
	update_status   Update(float dt) override;
	bool            CleanUp() override;

    void Draw(const float4x4& viewproj, unsigned width, unsigned height);
    void DrawAxis();
    void DrawFrustum(const Frustum& frustum);
    bool GetShouldRenderGrid() const { return mDrawGrid; }
    void SetRenderGrid(bool a) { mDrawGrid = a; }
    void DrawCube(const OBB& obb, const float3& color);
    void DrawSphere(const float center[3], const float color[3], const float radius);
    void DrawCone(const float pos[3], const float dir[3], const float color[3], const float bRadius);
    void DrawLine(const float3& position, const float3& direction, const float3& color);
    void DrawLine(const float3& start, const float3& end, const float3& color, float duration, bool depthTest);
    void DrawTriangle(const float3& v1, const float3& v2, const float3& v3);
    void DrawTriangle(const float3& v1, const float3& v2, const float3& v3, const float3& color);
    void DrawCircle(const float3 center, const float radius);
    void DrawSkeleton(GameObject* model);
    void DrawColliders(GameObject* root);
    void DrawBoundingBoxes(GameObject* gameObject);

private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mDrawGrid = true;

    void DrawGrid();
    


};
#endif /* _MODULE_DEBUGDRAW_H_ */
