#pragma once
#include "Script.h"
#include "Macros.h"

#define SERIALIZE_FIELD
// To create a script you have to locate the .h in Game/Assets/Scripts

extern "C" class SCRIPTING_API TestScript : public Script {

public:
    TestScript(GameObject* owner);
    ~TestScript() {}
    void Start() override;
    void Update() override;

private:
    SERIALIZE_FIELD float speed = 1;
    SERIALIZE_FIELD float coolDown = 2;
    SERIALIZE_FIELD float height = 1;
    SERIALIZE_FIELD float movement = 0;
    SERIALIZE_FIELD bool up = false, startCounter = false;
    float timePassed = 0;

    GameObject* gameObject;

};

extern "C" SCRIPTING_API Script* CreateTestScript(GameObject* owner) { return new TestScript(owner); }

//extern "C" SCRIPTING_API void fibonacci_init2(const unsigned long long a, const unsigned long long b);
