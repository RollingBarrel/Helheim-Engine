#pragma once
#include "Panel.h"
#define NAVMESHPANEL "NavMesh Parameters"
class NavMeshController;
class NavMeshControllerPanel :
    public Panel
{
public:
    NavMeshControllerPanel();
    ~NavMeshControllerPanel();

    void Draw(int windowFlags) override;

    

private:
    void DrawDebugPolyMesh();
    void LoadMeshVAO();

    NavMeshController* mNavController = nullptr;
    bool mDraw = false;
    unsigned int mVao = 0;
    unsigned int mVbo = 0;
    unsigned int mEbo = 0;
};

