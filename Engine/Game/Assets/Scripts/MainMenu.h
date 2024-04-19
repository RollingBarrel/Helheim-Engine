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
    void ChangeImage(const char* name, bool enabled) const;
    void Controls();

    bool mActiveMenu = true;
    bool mPauseMenu = false;
    bool mResetScreen = true;
    int mImage = 1;
    const char* mActualImageName = nullptr;
    const char* mPreviousImageName = nullptr;
};

