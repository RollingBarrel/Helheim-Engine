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

    //void TakeDamage(float damage);

private:
    float mTest = 100.0f;
};
