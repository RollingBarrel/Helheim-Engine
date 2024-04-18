#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(GameManager);
class GameManager : public Script
{
    FRIEND(GameManager)
public:
    GameManager(GameObject* owner);
    ~GameManager() {}
    void Start() override;
    void Update() override;

private:
    bool mSplashScreen = false;
};