#pragma once
#include "Script.h"
#include "Macros.h"
#include "Math/float3.h"

// To create a script you have to locate the .h in Game/Assets/Scripts

class TestScript : public Script {

public:
    TestScript(GameObject* owner);
    ~TestScript() {}
    void Start() override;
    void Update() override;
    float mPlayerSpeed = 1;
    float speed = 1;
    float3 mTestFloat3 = float3(5,15,-4);
    GameObject* mRobot = nullptr;
private:
    float coolDown = 2; 
    float height = 1;
    float movement = 0;
    bool up = false;
    bool startCounter = false;
    float timePassed = 0;

    
};

