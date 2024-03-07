#pragma once
#include "Script.h"
#include "Macros.h"

#define SERIALIZED_FIELD
#define DISPLAY
#define TOOLTIP(tooltip)
#define RANGE(min,max)
#define HEADER(header) 
#define SPACE 

// To create a script you have to locate the .h in Game/Assets/Scripts

extern "C" class SCRIPTING_API TestScript : public Script {

public:
    TestScript(GameObject* owner);
    ~TestScript() {}
    void Start() override;
    void Update() override;

private:
    HEADER("My Header")
    RANGE (1,100)
    DISPLAY float speed = 1;
    TOOLTIP("This is a tooltip for the next field.")
    DISPLAY float coolDown = 2;
    SPACE
    DISPLAY float height = 1;
    DISPLAY float movement = 0;
    DISPLAY bool up = false;
    bool startCounter = false;
    SERIALIZED_FIELD float timePassed = 0;

    GameObject* gameObject;
};

extern "C" SCRIPTING_API Script* CreateTestScript(GameObject* owner) { return new TestScript(owner); }

//extern "C" SCRIPTING_API void fibonacci_init2(const unsigned long long a, const unsigned long long b);
