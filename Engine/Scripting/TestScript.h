#pragma once
#include "Script.h"
#include "Macros.h"

extern "C" class SCRIPTING_API TestScript : public Script {

public:

    TestScript(GameObject* owner);
    ~TestScript() {}
    void Start() override;
    void Update() override;

private:

    float speed = 1;
    float coolDown = 2;
    float height = 1;
    float movement = 0;
    bool up = false, startCounter = false;
    float timePassed = 0;

    GameObject* gameObject;

#define SCRIPT_DECLARATION(type, name, value)\
private:\
    type name = value;\
    listOfAtributes.push_back(ScriptAttribute(name, type, value));
    

    SCRIPT_DECLARATION(float, fast, &fast)

};


extern "C" SCRIPTING_API Script* CreateTestScript(GameObject* owner) { return new TestScript(owner); }

//extern "C" SCRIPTING_API void fibonacci_init2(const unsigned long long a, const unsigned long long b);
