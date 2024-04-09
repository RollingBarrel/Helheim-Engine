#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"
class NavMeshController;
class AnimationComponent;

BODY(PlayerController);
class PlayerController :public Script
{
    FRIEND(PlayerController)
public:
    PlayerController(GameObject* owner);
    ~PlayerController() {}
    void Start() override;
    void Update() override;
    void Move();
    void Win();
    void Lose();
    void Rotate();
    void Dash();
    float mPlayerSpeed = 1;
    float mPlayerRotationSpeed = 1.0f;
    GameObject* mWinArea = nullptr;
    GameObject* mLoseArea = nullptr;
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





