#pragma once
#include "Script.h"
#include "Macros.h"
#include "Math/float3.h"

// To create a script you have to locate the .h in Game/Assets/Scripts
// To show it in the inspector, drago and drop the .h file into the Project Panel

GENERATE_BODY(TestScript);
class TestScript : public Script 
{
    FRIEND(TestScript)
public:
    TestScript(GameObject* owner) : Script(owner) {}
    ~TestScript() {}
    void Start() override;
    void Update() override;

    void OnButtonClick() override;
   
private:
    float mPlayerSpeed = 1;
    float mSpeed = 1;
    float3 mTestFloat3 = float3(5, 15, -4);
    GameObject* mRobot = nullptr;
    float mCoolDown = 2; 
    float mHeight = 1;
    float mMovement = 0;
    bool mUp = false;
    bool mStartCounter = false;
    float mTimePassed = 0;

    
};

