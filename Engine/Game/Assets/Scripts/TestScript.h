#pragma once
#include "Script.h"
#include "Macros.h"

//#define GET_VARIABLE_NAME(Variable) (#Variable)

// To create a script you have to locate the .h in Game/Assets/Scripts

class TestScript : public Script {
    friend class InspectorPanel;

public:
    TestScript(GameObject* owner);
    ~TestScript() {}
    void Start() override;
    void Update() override;
    float mPlayerSpeed = 1;
    float speed = 1;

private:
    float coolDown = 2; 
    float height = 1;
    float movement = 0;
    bool up = false;
    bool startCounter = false;
    float timePassed = 0;

    GameObject* gameObject;
};

extern "C" SCRIPTING_API Script* CreateTestScript(GameObject* owner, std::vector<ScriptVariable*>& data) 
{

    TestScript* script = new TestScript(owner);
   
    data.push_back(new ScriptVariable("mPlayerSpeed", VariableType::FLOAT, &script->mPlayerSpeed));
    data.push_back(new ScriptVariable("speed", VariableType::FLOAT, &script->speed));


    return script;
}