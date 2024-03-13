#pragma once
#include "Panel.h"
#define NAVMESHPANEL "NavMesh Parameters"

class NavMeshControllerPanel :
    public Panel
{
public:
    NavMeshControllerPanel();
    ~NavMeshControllerPanel();

    void Draw(int windowFlags) override;

};

