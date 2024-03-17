#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"
class NavMeshController;
class Movement :public Script
{
public:
    Movement(GameObject* owner);
    ~Movement() {}
    void Start() override;
    void Update() override;
    float mPlayerSpeed = 1;
private:
    NavMeshController* mNavMeshControl = nullptr;
};

extern "C" SCRIPTING_API Script * CreateMovement(GameObject * owner, std::vector<ScriptVariable*>&data)
{

    Movement* script = new Movement(owner);

    data.push_back(new ScriptVariable("mPlayerSpeed", VariableType::FLOAT, &script->mPlayerSpeed));
    return script;
}



