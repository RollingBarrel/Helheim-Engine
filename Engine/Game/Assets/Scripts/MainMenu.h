#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(MainMenu);
class MainMenu : public Script
{
    FRIEND(MainMenu)
public:
    MainMenu(GameObject* owner);
    ~MainMenu() {}
    void Start() override;
    void Update() override;

    //void TakeDamage(float damage);

private:
    float mTest = 100.0f;
};

