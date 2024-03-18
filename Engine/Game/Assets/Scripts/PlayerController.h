#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"
class NavMeshController;
class AnimationComponent;

class PlayerController :public Script
{
public:
    PlayerController(GameObject* owner);
    ~PlayerController() {}
    void Start() override;
    void Update() override;
    void Move();
    void Win();
    void Rotate();
    void Dash();
    float mPlayerSpeed = 1;
    float mPlayerRotationSpeed = 1.0f;
    GameObject* mWinArea = nullptr;
    GameObject* mAnimationComponentHolder = nullptr;
    float mDashSpeed = 5.0f;
    float mDashLenght = 5.0f;
    float mDashCoolDown = 1.0f;

private:
    NavMeshController* mNavMeshControl = nullptr;
    AnimationComponent* mAnimationComponent = nullptr;
    bool mIsDashActive = false;
    bool mStartCounter = false;
    float mDashTimePassed = 0.0f;
    float mDashMovement = 0;

};

extern "C" SCRIPTING_API Script * CreatePlayerController(GameObject * owner, std::vector<ScriptVariable*>&data)
{

    PlayerController* script = new PlayerController(owner);

    data.push_back(new ScriptVariable("mPlayerSpeed", VariableType::FLOAT, &script->mPlayerSpeed));
    data.push_back(new ScriptVariable("mPlayerRotationSpeed", VariableType::FLOAT, &script->mPlayerRotationSpeed));
    data.push_back(new ScriptVariable("mWinArea", VariableType::GAMEOBJECT, &script->mWinArea));
    data.push_back(new ScriptVariable("mAnimationComponentHolder", VariableType::GAMEOBJECT, &script->mAnimationComponentHolder));

    data.push_back(new ScriptVariable("mDashSpeed", VariableType::FLOAT, &script->mDashSpeed));
    data.push_back(new ScriptVariable("mDashLenght", VariableType::FLOAT, &script->mDashLenght));
    data.push_back(new ScriptVariable("mDashCoolDown", VariableType::FLOAT, &script->mDashCoolDown));

    return script;
}



