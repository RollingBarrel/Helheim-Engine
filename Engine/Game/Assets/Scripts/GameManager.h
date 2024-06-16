#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;
class AudioManager;

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
    AudioManager* GetAudio() {return mAudioManager;};

    void LoadLevel(const char* LevelName);
private:
    GameManager(GameObject* owner); 
    ~GameManager();

    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;
    GameObject* mAudioManagerGO = nullptr;

    HudController* mHudController = nullptr;
    AudioManager* mAudioManager = nullptr;

    bool mPaused = false;
};