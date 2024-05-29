#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;

GENERATE_BODY(GameManager);
class GameManager : public Script
{
    FRIEND(GameManager)
public:
    static GameManager* GetInstance(); 

    void Awake();
    void Start();
    void Update();

    GameObject* GetPlayer() { return mPlayer; };

    void LoadLevel(const char* LevelName);
private:
    GameManager(GameObject* owner); 
    ~GameManager();

    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;

    HudController* mHudController = nullptr;

    bool mPaused = false;
};