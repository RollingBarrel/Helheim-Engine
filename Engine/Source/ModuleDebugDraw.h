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

	bool            Init() override;
	update_status   Update() override;
	bool            CleanUp() override;

    void SetDrawGrid(bool drawGrid) { mdrawGrid = drawGrid; };
    bool GetDrawGrid() const { return mdrawGrid; };
    void Draw(const float4x4& viewproj, unsigned width, unsigned height);


private:

    static DDRenderInterfaceCoreGL* implementation;
    bool mdrawGrid; 


    void DrawBoundingBox(const OBB& obb);
    void DrawGrid();
    void DrawFrustum(const Frustum& frustum); 

};

#endif /* _MODULE_DEBUGDRAW_H_ */
