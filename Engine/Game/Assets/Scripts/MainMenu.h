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

    void SetMenu(bool active, bool pause);

private:
    void MenuStatus();
    void Controls();

    bool mActiveMenu;
    bool mPauseMenu;
    bool mResetScreen = false;
    int mScreen = 1;
};

